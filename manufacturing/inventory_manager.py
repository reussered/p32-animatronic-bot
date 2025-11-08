#!/usr/bin/env python3
"""
Inventory Management & Automatic Ordering System
P32 Animatronic Bot Project

This script manages component inventory, tracks stock levels, suggests reorders,
and can integrate with vendor APIs for automatic purchasing.
"""

import json
import sys
from datetime import datetime
from typing import Dict, List, Optional
from dataclasses import dataclass
from enum import Enum

class OrderStatus(Enum):
    """Possible states for an order"""
    DRAFT = "draft"
    PENDING = "pending"
    ORDERED = "ordered"
    SHIPPED = "shipped"
    RECEIVED = "received"
    CANCELLED = "cancelled"

@dataclass
class InventoryItem:
    """Represents a single component in inventory"""
    item_id: str
    category: str
    name: str
    description: str
    manufacturer: str
    manufacturer_sku: str
    current_stock: int
    unit_cost: float
    reorder_point: int  # Trigger automatic reorder when stock drops below this
    reorder_quantity: int  # How many to order each time
    lead_time_days: int  # How long until delivery
    preferred_vendors: List[str]  # Ordered list of preferred suppliers
    last_reorder_date: Optional[str] = None
    notes: str = ""

@dataclass
class Vendor:
    """Represents a supplier/vendor"""
    vendor_id: str
    name: str
    website: str
    api_endpoint: Optional[str]  # For automated ordering
    api_key: Optional[str]  # Encrypted in real implementation
    shipping_cost_base: float
    shipping_threshold_free: float  # Free shipping above this amount
    average_lead_time_days: int
    reliability_score: float  # 0-100, based on on-time delivery
    accepted_payment_methods: List[str]
    supported_components: List[str]  # Categories they supply

class InventoryManager:
    """Manages inventory and automatic ordering"""
    
    def __init__(self, inventory_file: str):
        self.inventory_file = inventory_file
        self.data = self._load_inventory()
        self.items: Dict[str, InventoryItem] = {}
        self.vendors: Dict[str, Vendor] = {}
        self._parse_inventory()
    
    def _load_inventory(self) -> dict:
        """Load inventory from JSON file"""
        try:
            with open(self.inventory_file, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            print(f"Error: Inventory file not found: {self.inventory_file}")
            sys.exit(1)
    
    def _parse_inventory(self):
        """Parse loaded JSON into typed objects"""
        # Parse items
        for category, category_data in self.data['inventory']['categories'].items():
            for item_data in category_data.get('items', []):
                item = InventoryItem(**item_data)
                self.items[item.item_id] = item
        
        # Parse vendors
        for vendor_data in self.data['inventory']['vendors']:
            vendor = Vendor(**vendor_data)
            self.vendors[vendor.vendor_id] = vendor
    
    def check_reorder_needed(self) -> List[str]:
        """
        Identify items that need reordering based on stock levels.
        Returns list of item IDs that should be reordered.
        """
        reorder_list = []
        for item_id, item in self.items.items():
            if item.current_stock <= item.reorder_point:
                reorder_list.append(item_id)
                print(f"⚠️  REORDER NEEDED: {item.name}")
                print(f"   Current: {item.current_stock}, Minimum: {item.reorder_point}")
        return reorder_list
    
    def calculate_best_vendor(self, item: InventoryItem) -> Optional[Vendor]:
        """
        Select best vendor for an item based on:
        - Price
        - Availability (in stock)
        - Shipping cost
        - Reliability score
        - Lead time
        """
        candidates = []
        for vendor_id in item.preferred_vendors:
            if vendor_id in self.vendors:
                vendor = self.vendors[vendor_id]
                # Score calculation (higher is better)
                score = vendor.reliability_score
                score -= vendor.average_lead_time_days * 2  # Penalize long lead times
                candidates.append((score, vendor))
        
        if not candidates:
            return None
        
        candidates.sort(key=lambda x: x[0], reverse=True)
        return candidates[0][1]
    
    def generate_purchase_order(self, item_id: str) -> Dict:
        """Generate a purchase order for a specific item"""
        if item_id not in self.items:
            print(f"Error: Item not found: {item_id}")
            return None
        
        item = self.items[item_id]
        vendor = self.calculate_best_vendor(item)
        
        if not vendor:
            print(f"Error: No preferred vendor available for {item.name}")
            return None
        
        quantity = item.reorder_quantity
        unit_cost = item.unit_cost
        subtotal = quantity * unit_cost
        
        # Calculate shipping (simplified)
        shipping = 0
        if subtotal < vendor.shipping_threshold_free:
            shipping = vendor.shipping_cost_base
        
        total = subtotal + shipping
        
        purchase_order = {
            "po_number": f"PO-{item_id}-{datetime.now().strftime('%Y%m%d%H%M%S')}",
            "date": datetime.now().isoformat(),
            "item_id": item_id,
            "item_name": item.name,
            "quantity": quantity,
            "unit_cost": unit_cost,
            "subtotal": subtotal,
            "vendor_id": vendor.vendor_id,
            "vendor_name": vendor.name,
            "shipping_cost": shipping,
            "total": total,
            "expected_delivery": datetime.now().replace(day=datetime.now().day + vendor.average_lead_time_days).isoformat(),
            "status": OrderStatus.DRAFT.value
        }
        
        return purchase_order
    
    def generate_bulk_purchase_order(self, item_ids: List[str]) -> Dict:
        """
        Generate a consolidated purchase order for multiple items from same vendor
        to save on shipping costs.
        """
        orders = []
        total_cost = 0
        vendor_groups = {}
        
        # Group items by preferred vendor
        for item_id in item_ids:
            item = self.items[item_id]
            vendor = self.calculate_best_vendor(item)
            if not vendor:
                continue
            
            if vendor.vendor_id not in vendor_groups:
                vendor_groups[vendor.vendor_id] = {'vendor': vendor, 'items': []}
            
            vendor_groups[vendor.vendor_id]['items'].append((item, item.reorder_quantity))
        
        # Create consolidated orders by vendor
        consolidated_orders = []
        for vendor_id, group in vendor_groups.items():
            vendor = group['vendor']
            subtotal = sum(item.unit_cost * qty for item, qty in group['items'])
            
            # Calculate shipping
            shipping = 0
            if subtotal < vendor.shipping_threshold_free:
                shipping = vendor.shipping_cost_base
            
            total = subtotal + shipping
            
            order = {
                "po_number": f"BULK-{vendor_id}-{datetime.now().strftime('%Y%m%d%H%M%S')}",
                "date": datetime.now().isoformat(),
                "vendor_id": vendor_id,
                "vendor_name": vendor.name,
                "items": [
                    {
                        "item_id": item.item_id,
                        "name": item.name,
                        "quantity": qty,
                        "unit_cost": item.unit_cost,
                        "subtotal": item.unit_cost * qty
                    }
                    for item, qty in group['items']
                ],
                "subtotal": subtotal,
                "shipping_cost": shipping,
                "total": total,
                "status": OrderStatus.DRAFT.value
            }
            consolidated_orders.append(order)
            total_cost += total
        
        return {
            "bulk_order_date": datetime.now().isoformat(),
            "total_items": len(item_ids),
            "total_cost": total_cost,
            "orders": consolidated_orders
        }
    
    def print_inventory_report(self):
        """Print human-readable inventory report"""
        print("\n" + "="*80)
        print("INVENTORY REPORT")
        print("="*80)
        
        by_category = {}
        for item_id, item in self.items.items():
            if item.category not in by_category:
                by_category[item.category] = []
            by_category[item.category].append(item)
        
        for category, items in by_category.items():
            print(f"\n📦 {category.upper()}")
            print("-" * 80)
            for item in items:
                status = "✅" if item.current_stock > item.reorder_point else "⚠️ "
                print(f"{status} {item.name}")
                print(f"   Manufacturer: {item.manufacturer} ({item.manufacturer_sku})")
                print(f"   Stock: {item.current_stock} (reorder point: {item.reorder_point})")
                print(f"   Unit Cost: ${item.unit_cost:.2f}")
                print(f"   Lead Time: {item.lead_time_days} days")
                if item.notes:
                    print(f"   Notes: {item.notes}")
        
        print("\n" + "="*80)
    
    def print_reorder_summary(self):
        """Print items needing reorder"""
        reorder_items = self.check_reorder_needed()
        
        if not reorder_items:
            print("\n✅ All items are adequately stocked!")
            return
        
        print(f"\n⚠️  {len(reorder_items)} items need reordering:")
        total_cost = 0
        for item_id in reorder_items:
            item = self.items[item_id]
            cost = item.reorder_quantity * item.unit_cost
            total_cost += cost
            print(f"  • {item.name}: order {item.reorder_quantity} units @ ${item.unit_cost}/ea = ${cost:.2f}")
        
        print(f"\nEstimated total cost (before shipping): ${total_cost:.2f}")

def main():
    """Example usage"""
    manager = InventoryManager('manufacturing/inventory.json')
    
    print("P32 Animatronic Bot - Inventory Manager")
    print("=" * 80)
    
    # Print current inventory
    manager.print_inventory_report()
    
    # Check what needs reordering
    print("\n")
    manager.print_reorder_summary()
    
    # Generate sample bulk purchase order
    reorder_items = manager.check_reorder_needed()
    if reorder_items:
        print("\n📋 Generating bulk purchase order...")
        bulk_order = manager.generate_bulk_purchase_order(reorder_items)
        print(json.dumps(bulk_order, indent=2))

if __name__ == "__main__":
    main()
