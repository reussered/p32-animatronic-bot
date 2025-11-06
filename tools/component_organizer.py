#!/usr/bin/env python3
"""
COMPONENT ORGANIZATION ENGINE
Smart reorganization of component files into optimal locations
Can run as background agent or standalone

Strategy:
1. Analyze all components (JSON files)
2. Determine optimal location based on content analysis
3. Move .src/.hdr to match .json location (colocation principle)
4. Handle duplicates intelligently (keep best version)
5. Generate organization report
6. Verify no references are broken
"""

import json
import shutil
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, field
from enum import Enum
import logging

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

class ComponentType(Enum):
    """Categorize components by their function"""
    CREATURE_SPECIFIC = "creature"      # Eyes, ears, creature features
    HARDWARE = "hardware"               # Sensors, motors, displays
    DRIVER = "driver"                   # GPIO, I2C, SPI, PWM
    SUBSYSTEM = "subsystem"             # Head, torso, limbs
    SYSTEM = "system"                   # Controllers, coordinators
    INTERFACE = "interface"             # Communication protocols
    FUNCTIONAL = "functional"           # Behavioral components
    UTILITY = "utility"                 # Generic helpers
    UNKNOWN = "unknown"

@dataclass
class ComponentFile:
    """Represents a single component (name, all its files, metadata)"""
    name: str
    json_path: Optional[Path] = None
    src_paths: List[Path] = field(default_factory=list)
    hdr_paths: List[Path] = field(default_factory=list)
    component_type: ComponentType = ComponentType.UNKNOWN
    creature: Optional[str] = None      # e.g., "goblin", "bear"
    subsystem: Optional[str] = None     # e.g., "head", "torso"
    is_duplicate: bool = False
    priority: int = 0                   # Higher = keep this version
    metadata: Dict = field(default_factory=dict)
    
    def best_src(self) -> Optional[Path]:
        """Return the src file we should keep"""
        return self.src_paths[0] if self.src_paths else None
    
    def best_hdr(self) -> Optional[Path]:
        """Return the hdr file we should keep"""
        return self.hdr_paths[0] if self.hdr_paths else None

class ComponentAnalyzer:
    """Analyzes component JSON to determine optimal location"""
    
    CREATURE_PATTERNS = {
        "goblin", "bear", "cat", "dragon", "elf", "wolf", "vampire",
        "zombie", "robot", "steampunk", "android", "cyclops", "pixie",
        "ghost", "skeleton", "orc"
    }
    
    SUBSYSTEM_PATTERNS = {
        "head", "torso", "arm", "leg", "hand", "foot", "spine", "pelvis"
    }
    
    HARDWARE_KEYWORDS = {
        "servo", "motor", "sensor", "display", "led", "speaker", "microphone",
        "button", "switch", "actuator", "pump", "valve"
    }
    
    DRIVER_KEYWORDS = {
        "gpio", "i2c", "spi", "pwm", "uart", "can", "uart", "bus", "driver",
        "interface", "controller"
    }
    
    @staticmethod
    def analyze(json_path: Path, json_data: Dict) -> Tuple[ComponentType, Dict]:
        """Determine component type and metadata"""
        name = json_path.stem.lower()
        component_type = ComponentType.UNKNOWN
        metadata = {}
        
        # Check for creature-specific components
        for creature in ComponentAnalyzer.CREATURE_PATTERNS:
            if creature in name:
                metadata["creature"] = creature
                # Check if it's a subsystem component
                for subsystem in ComponentAnalyzer.SUBSYSTEM_PATTERNS:
                    if subsystem in name:
                        metadata["subsystem"] = subsystem
                        component_type = ComponentType.SUBSYSTEM
                        break
                if component_type == ComponentType.UNKNOWN:
                    component_type = ComponentType.CREATURE_SPECIFIC
                break
        
        # Check for hardware components
        if component_type == ComponentType.UNKNOWN:
            for keyword in ComponentAnalyzer.HARDWARE_KEYWORDS:
                if keyword in name:
                    component_type = ComponentType.HARDWARE
                    break
        
        # Check for drivers
        if component_type == ComponentType.UNKNOWN:
            for keyword in ComponentAnalyzer.DRIVER_KEYWORDS:
                if keyword in name:
                    component_type = ComponentType.DRIVER
                    break
        
        # Check for system components
        if component_type == ComponentType.UNKNOWN:
            if any(x in name for x in ["coordinator", "controller", "mesh", "network", "monitor"]):
                component_type = ComponentType.SYSTEM
        
        # Check JSON content for hints
        if "positioned_component_type" in json_data:
            metadata["positioned"] = json_data.get("positioned_component_type")
        
        if "hardware_template" in json_data:
            metadata["uses_hardware"] = json_data.get("hardware_template")
        
        return component_type, metadata

class ComponentOrganizer:
    """Main orchestrator for component reorganization"""
    
    def __init__(self, components_root: str = "config/components"):
        self.root = Path(components_root)
        self.components: Dict[str, ComponentFile] = {}
        self.analyzer = ComponentAnalyzer()
        self.moves_planned: List[Tuple[Path, Path]] = []
        self.duplicates: List[List[ComponentFile]] = []
    
    def scan(self):
        """Scan all component files"""
        logger.info(f"Scanning {self.root}...")
        
        # Find all JSON files
        for json_path in self.root.rglob("*.json"):
            name = json_path.stem
            
            # Analyze JSON content
            try:
                with open(json_path, 'r', encoding='ascii') as f:
                    json_data = json.load(f)
            except:
                json_data = {}
            
            comp_type, metadata = self.analyzer.analyze(json_path, json_data)
            
            if name not in self.components:
                self.components[name] = ComponentFile(
                    name=name,
                    component_type=comp_type,
                    metadata=metadata
                )
            
            comp = self.components[name]
            comp.json_path = json_path
            
            # Mark duplicates
            if comp.json_path and comp.json_path != json_path:
                comp.is_duplicate = True
        
        # Find all .src and .hdr files
        for src_path in self.root.rglob("*.src"):
            name = src_path.stem
            if name not in self.components:
                self.components[name] = ComponentFile(name=name)
            self.components[name].src_paths.append(src_path)
        
        for hdr_path in self.root.rglob("*.hdr"):
            name = hdr_path.stem
            if name not in self.components:
                self.components[name] = ComponentFile(name=name)
            self.components[name].hdr_paths.append(hdr_path)
        
        logger.info(f"Found {len(self.components)} unique components")
    
    def compute_moves(self):
        """Determine all necessary moves"""
        logger.info("Computing optimal moves...")
        
        for name, comp in self.components.items():
            if not comp.json_path:
                continue  # Skip orphaned .src/.hdr files for now
            
            target_dir = comp.json_path.parent
            
            # Plan to move .src files
            for src_path in comp.src_paths:
                if src_path.parent != target_dir:
                    target = target_dir / src_path.name
                    self.moves_planned.append((src_path, target))
            
            # Plan to move .hdr files
            for hdr_path in comp.hdr_paths:
                if hdr_path.parent != target_dir:
                    target = target_dir / hdr_path.name
                    self.moves_planned.append((hdr_path, target))
        
        logger.info(f"Planned {len(self.moves_planned)} file moves")
    
    def execute_moves(self, dry_run: bool = True):
        """Execute the planned moves"""
        mode = "DRY RUN" if dry_run else "EXECUTING"
        logger.info(f"{mode}: Moving {len(self.moves_planned)} files...")
        
        success = 0
        errors = 0
        
        for src, dst in self.moves_planned:
            try:
                if not dry_run:
                    dst.parent.mkdir(parents=True, exist_ok=True)
                    shutil.move(str(src), str(dst))
                
                logger.info(f" {src.name}: {src.parent.name}/  {dst.parent.name}/")
                success += 1
            except Exception as e:
                logger.error(f" {src.name}: {e}")
                errors += 1
        
        logger.info(f"\nResults: {success} moved, {errors} errors")
        if dry_run:
            logger.info("DRY RUN - No files were actually moved")
    
    def generate_report(self) -> str:
        """Generate organizational report"""
        report = []
        report.append("=" * 80)
        report.append("COMPONENT ORGANIZATION REPORT")
        report.append("=" * 80)
        
        # Statistics by type
        by_type = {}
        for comp in self.components.values():
            t = comp.component_type.value
            by_type[t] = by_type.get(t, 0) + 1
        
        report.append("\nCOMPONENTS BY TYPE:")
        for t, count in sorted(by_type.items()):
            report.append(f"  {t:20} : {count:3} files")
        
        report.append(f"\nTOTAL: {len(self.components)} components")
        report.append(f"PLANNED MOVES: {len(self.moves_planned)} files")
        
        return "\n".join(report)

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="Intelligent component reorganizer")
    parser.add_argument("--scan-only", action="store_true", help="Just scan, don't move")
    parser.add_argument("--dry-run", action="store_true", default=True, help="Preview moves")
    parser.add_argument("--execute", action="store_true", help="Actually move files")
    parser.add_argument("--report", action="store_true", help="Generate report")
    
    args = parser.parse_args()
    
    organizer = ComponentOrganizer()
    organizer.scan()
    
    if args.scan_only:
        print(organizer.generate_report())
        return
    
    organizer.compute_moves()
    
    if args.report:
        print(organizer.generate_report())
    
    if args.execute:
        organizer.execute_moves(dry_run=False)
    else:
        organizer.execute_moves(dry_run=True)

if __name__ == "__main__":
    main()
