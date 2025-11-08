"""
Component Registry Utility
Maintains alphabetically sorted registry of all components.
Supports duplicates and tracks git repository source.
Uses binary search for efficient lookups.
"""

import json
import bisect
from pathlib import Path
from typing import List, Dict, Optional, Tuple


class ComponentRegistry:
    """Binary search registry for components with duplicate tracking."""

    def __init__(self, registry_path: str = None):
        """Initialize registry from file or create empty."""
        if registry_path is None:
            registry_path = "config/component_registry.json"
        self.registry_path = Path(registry_path)
        self.data = self._load()

    def _load(self) -> Dict:
        """Load registry from JSON file."""
        if self.registry_path.exists():
            with open(self.registry_path, 'r', encoding='utf-8') as f:
                return json.load(f)
        return {
            "version": "1.0.0",
            "description": "Master component registry",
            "last_updated": "",
            "components": []
        }

    def save(self) -> None:
        """Save registry to JSON file."""
        from datetime import datetime
        self.data["last_updated"] = datetime.now().isoformat()
        with open(self.registry_path, 'w', encoding='utf-8') as f:
            json.dump(self.data, f, indent=2)

    def _get_name_index(self, name: str) -> int:
        """Get insertion point for name using binary search."""
        components = self.data["components"]
        names = [c["name"] for c in components]
        return bisect.bisect_left(names, name)

    def add(self, name: str, path: str, git_repo: str = "p32-animatronic-bot") -> None:
        """
        Add component to registry, maintaining alphabetical order.
        Allows duplicates.
        """
        components = self.data["components"]
        idx = self._get_name_index(name)

        entry = {
            "name": name,
            "path": path,
            "git_repo": git_repo
        }

        components.insert(idx, entry)

    def get_all(self, name: str) -> List[Dict]:
        """Get all entries with given name (handles duplicates)."""
        components = self.data["components"]
        names = [c["name"] for c in components]

        # Binary search for first occurrence
        idx = bisect.bisect_left(names, name)

        if idx >= len(names) or names[idx] != name:
            return []

        # Collect all duplicates
        results = []
        while idx < len(names) and names[idx] == name:
            results.append(components[idx])
            idx += 1

        return results

    def get_first(self, name: str) -> Optional[Dict]:
        """Get first (earliest in git history or primary) entry for name."""
        results = self.get_all(name)
        return results[0] if results else None

    def get_latest(self, name: str, repo: str = None) -> Optional[Dict]:
        """
        Get entry for name, optionally filtered by repository.
        Last entry is considered most recent.
        """
        results = self.get_all(name)
        if not results:
            return None

        if repo:
            for entry in reversed(results):
                if entry["git_repo"] == repo:
                    return entry
            return None

        return results[-1]

    def remove(self, name: str, path: str) -> bool:
        """Remove specific component entry."""
        components = self.data["components"]
        for i, c in enumerate(components):
            if c["name"] == name and c["path"] == path:
                components.pop(i)
                return True
        return False

    def count_by_name(self, name: str) -> int:
        """Count how many entries exist for a name."""
        return len(self.get_all(name))

    def list_duplicates(self) -> Dict[str, int]:
        """Return names with duplicate counts > 1."""
        name_counts = {}
        for component in self.data["components"]:
            name = component["name"]
            name_counts[name] = name_counts.get(name, 0) + 1

        return {k: v for k, v in name_counts.items() if v > 1}

    def get_all_names(self) -> List[str]:
        """Get all unique component names in registry."""
        seen = set()
        names = []
        for c in self.data["components"]:
            if c["name"] not in seen:
                names.append(c["name"])
                seen.add(c["name"])
        return names

    def to_dict(self) -> Dict[str, List[Dict]]:
        """Export registry as dict keyed by name (for compatibility)."""
        result = {}
        for component in self.data["components"]:
            name = component["name"]
            if name not in result:
                result[name] = []
            result[name].append(component)
        return result


def binary_search_path(name: str, registry: ComponentRegistry) -> Optional[str]:
    """Quick lookup: get first matching path for component name."""
    entry = registry.get_first(name)
    return entry["path"] if entry else None


def binary_search_all_paths(name: str, registry: ComponentRegistry) -> List[str]:
    """Get all paths for component name (handles duplicates)."""
    entries = registry.get_all(name)
    return [e["path"] for e in entries]


if __name__ == "__main__":
    # Example usage
    reg = ComponentRegistry("config/component_registry.json")

    # Add some components
    reg.add("goblin_eye", "config/components/goblin_eye.json", "p32-animatronic-bot")
    reg.add("goblin_left_eye", "config/bots/bot_families/goblins/head/goblin_left_eye.json", "p32-animatronic-bot")
    reg.add("goblin_right_eye", "config/bots/bot_families/goblins/head/goblin_right_eye.json", "p32-animatronic-bot")

    # Save
    reg.save()

    # Lookup
    print(f"Found: {binary_search_path('goblin_eye', reg)}")
    print(f"Duplicates: {reg.list_duplicates()}")
