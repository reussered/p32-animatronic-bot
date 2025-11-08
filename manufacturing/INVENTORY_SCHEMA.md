# Inventory System Schema & Format

## JSON Structure

### Top-Level Format

```json
{
  "inventory": {
    "metadata": {
      "last_updated": "YYYY-MM-DD",
      "version": "1.0",
      "currency": "USD",
      "organization": "P32 Animatronic Bot Project"
    },
    "categories": { ... },
    "vendors": [ ... ],
    "supply_chain_rules": { ... }
  }
}
```

---

## Categories

Items are organized into categories. Each category has items array:

```json
"displays": {
  "category_name": "Display Modules",
  "description": "LCD/OLED screens and display controllers",
  "items": [
    {
      "item_id": "DISP_001",
      "category": "displays",
      "name": "GC9A01 240x240 Round LCD",
      "description": "1.28 inch circular IPS LCD display, RGB565, SPI interface",
      "manufacturer": "AMOLED/Display",
      "manufacturer_sku": "GC9A01-1.28in",
      "current_stock": 15,
      "unit_cost": 12.50,
      "reorder_point": 5,
      "reorder_quantity": 20,
      "lead_time_days": 14,
      "preferred_vendors": ["vendor_aliexpress", "vendor_amazon"],
      "last_reorder_date": "2025-10-15",
      "notes": "In stock at AliExpress. Good reliability."
    }
  ]
}
```

### Item Fields Explained

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `item_id` | string | Yes | Unique identifier (format: CATEGORY_###) |
| `category` | string | Yes | Must match category name |
| `name` | string | Yes | Display name (human readable) |
| `description` | string | No | Technical specs/details |
| `manufacturer` | string | Yes | Brand/manufacturer name |
| `manufacturer_sku` | string | Yes | Part number from manufacturer |
| `current_stock` | integer | Yes | Units currently in inventory |
| `unit_cost` | float | Yes | Price per unit in USD |
| `reorder_point` | integer | Yes | Auto-reorder when stock ≤ this value |
| `reorder_quantity` | integer | Yes | How many units to order each reorder |
| `lead_time_days` | integer | Yes | Days from order to delivery |
| `preferred_vendors` | array | Yes | Ordered list of vendor IDs to try |
| `last_reorder_date` | string | No | ISO date of last reorder |
| `notes` | string | No | Any additional notes |

---

## Vendors

Vendor definitions for automatic ordering:

```json
"vendors": [
  {
    "vendor_id": "vendor_aliexpress",
    "name": "AliExpress",
    "website": "https://www.aliexpress.com",
    "api_endpoint": null,
    "api_key": null,
    "shipping_cost_base": 2.50,
    "shipping_threshold_free": 50.00,
    "average_lead_time_days": 14,
    "reliability_score": 85.0,
    "accepted_payment_methods": ["credit_card", "paypal"],
    "supported_components": ["displays", "motors", "microcontrollers", "sensors", "misc"]
  },
  {
    "vendor_id": "vendor_mouser",
    "name": "Mouser Electronics",
    "website": "https://www.mouser.com",
    "api_endpoint": "https://api.mouser.com/v1/",
    "api_key": "ENCRYPTED_API_KEY",
    "shipping_cost_base": 5.00,
    "shipping_threshold_free": 100.00,
    "average_lead_time_days": 3,
    "reliability_score": 98.0,
    "accepted_payment_methods": ["credit_card", "purchase_order"],
    "supported_components": ["drivers", "microcontrollers", "power", "connectors"]
  }
]
```

### Vendor Fields Explained

| Field | Type | Description |
|-------|------|-------------|
| `vendor_id` | string | Unique identifier (format: vendor_NAME) |
| `name` | string | Display name |
| `website` | string | Vendor website URL |
| `api_endpoint` | string/null | REST API endpoint (for future automation) |
| `api_key` | string/null | API credentials (encrypted in production) |
| `shipping_cost_base` | float | Base shipping cost in USD |
| `shipping_threshold_free` | float | Free shipping above this order total |
| `average_lead_time_days` | integer | Typical days to delivery |
| `reliability_score` | float | 0-100, on-time delivery rate |
| `accepted_payment_methods` | array | Supported payment types |
| `supported_components` | array | Categories this vendor supplies |

---

## Supply Chain Rules

Define automatic reordering behavior:

```json
"supply_chain_rules": {
  "auto_reorder_rules": [
    {
      "rule_id": "rule_001",
      "rule_name": "Critical Components Weekly Check",
      "priority": "high",
      "frequency": "weekly",
      "target_items": ["MICRO_001", "MICRO_002"],
      "min_stock_threshold": 10,
      "action": "create_purchase_order"
    },
    {
      "rule_id": "rule_002",
      "rule_name": "Standard Components Monthly Check",
      "priority": "medium",
      "frequency": "monthly",
      "target_items": ["displays", "motors"],
      "min_stock_threshold": 5,
      "action": "send_notification"
    }
  ],
  "supplier_preferences": [
    {
      "preference_id": "pref_001",
      "description": "Prefer local suppliers when possible",
      "weight": 0.2,
      "criteria": "lead_time_days < 7"
    },
    {
      "preference_id": "pref_002",
      "description": "Minimize shipping costs",
      "weight": 0.3,
      "criteria": "vendor.shipping_cost_base < 5"
    },
    {
      "preference_id": "pref_003",
      "description": "Prefer reliable vendors",
      "weight": 0.5,
      "criteria": "vendor.reliability_score > 90"
    }
  ]
}
```

---

## Data Entry Template

When entering a new item, use this template:

```json
{
  "item_id": "CATEGORY_###",
  "category": "category_name",
  "name": "Human-readable product name",
  "description": "Technical specifications",
  "manufacturer": "Brand name",
  "manufacturer_sku": "Part number",
  "current_stock": 0,
  "unit_cost": 0.00,
  "reorder_point": 5,
  "reorder_quantity": 20,
  "lead_time_days": 14,
  "preferred_vendors": ["vendor_id_1", "vendor_id_2"],
  "last_reorder_date": null,
  "notes": ""
}
```

---

## Item ID Naming Scheme

Format: `CATEGORY_NUMBER`

Examples:
- `DISP_001` - First display item
- `DISP_002` - Second display item
- `MOTOR_001` - First motor
- `DRIVER_001` - First driver
- `MICRO_001` - First microcontroller
- `POWER_001` - First power supply
- `SENSOR_001` - First sensor
- `CONN_001` - First connector
- `MISC_001` - First miscellaneous item

---

## Categories Reference

| Category Key | Full Name | Examples |
|--------------|-----------|----------|
| `displays` | Display Modules | LCD, OLED, E-ink |
| `motors` | Motors & Actuators | Servo, Stepper, DC motor |
| `drivers` | Motor Drivers & Controllers | PWM, H-bridge, Servo controller |
| `microcontrollers` | Microcontroller Units | ESP32, ESP32-S3, STM32 |
| `power` | Power Supply & Management | LiPo battery, Regulator, Buck converter |
| `sensors` | Sensors | Temperature, IR, Accelerometer |
| `connectors` | Connectors & Cables | Headers, JST, Dupont connectors |
| `misc` | Miscellaneous | Resistors, capacitors, breadboards |

---

## Usage Examples

### Add a New Item

When you provide inventory data, format like:

```
DISPLAYS:
- GC9A01 240x240 Round LCD, Manufacturer: AMOLED/Display, SKU: GC9A01-1.28in, Stock: 15, Cost: $12.50

MOTORS:
- SG90 Micro Servo, Manufacturer: Tower Pro, SKU: SG90, Stock: 50, Cost: $3.20
```

The system will automatically:
1. Assign item_id (DISP_001, MOTOR_001, etc.)
2. Add to appropriate category
3. Set default reorder_point and reorder_quantity
4. Match to preferred vendors

### Check Inventory Status

```bash
python manufacturing/inventory_manager.py --report
```

Output:
```
INVENTORY REPORT
================
📦 DISPLAYS
✅ GC9A01 240x240 Round LCD
   Stock: 15 (reorder point: 5)
   Unit Cost: $12.50

⚠️  📦 MOTORS
⚠️  SG90 Micro Servo
   Stock: 3 (reorder point: 5)
   Unit Cost: $3.20
```

### Generate Purchase Orders

```bash
python manufacturing/inventory_manager.py --reorder
```

Output:
```
⚠️  2 items need reordering:
  • SG90 Micro Servo: order 20 units @ $3.20/ea = $64.00
  • ESP32-S3: order 10 units @ $8.50/ea = $85.00

Estimated total cost (before shipping): $149.00
```

---

## Ready for Input

I'm now ready for your inventory data. Please provide in any format:
- CSV (item name, qty, cost, sku)
- Spreadsheet format
- Natural language
- Existing document

I'll parse and populate the JSON automatically.

**What format is your inventory data in?**

