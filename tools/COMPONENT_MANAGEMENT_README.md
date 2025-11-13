# Component Management Tools

These tools help manage component lifecycle: registration, validation, and deletion.

## Tools

### `register_component.py`
Validates component files, registers to component registry, and commits to git.

**Features:**
- ✅ Validates component .json, .src, and .hdr files exist
- ✅ Handles controller components (no .src/.hdr required)
- ✅ Validates all component dependencies exist
- ✅ Adds component to component_registry.json
- ✅ Commits all files to git with proper commit message

**Usage:**
```bash
# Auto-find component in standard locations
python tools/register_component.py goblin_left_ear

# Specify custom path
python tools/register_component.py my_sensor --path config/components/hardware/my_sensor.json
```

**Standard search locations:**
- `config/components/hardware/`
- `config/components/drivers/`
- `config/components/behaviors/`
- `config/components/functional/`
- `config/components/interfaces/`
- `config/bots/bot_families/*/` (recursive)

### `delete_component.py`
Removes component from registry, archives files to `config/delete-saved/`, and commits to git.

**Features:**
- ✅ Finds component files automatically
- ✅ Checks if component is still referenced in any bot configs
- ✅ Prevents deletion if component is in use
- ✅ Removes from component_registry.json
- ✅ **Archives files to `config/delete-saved/` with `-saved` extensions** (safe recovery)
- ✅ Optionally unregisters without archiving (--keep-files)
- ✅ Requires confirmation before archiving
- ✅ Commits changes to git

**Usage:**
```bash
# Archive component (moves to config/delete-saved/)
python tools/delete_component.py goblin_left_ear

# Unregister but keep files in original location
python tools/delete_component.py my_sensor --keep-files
```

**Safety features:**
- Scans all .json files for component references
- Lists all files using the component
- Archives files instead of deleting (*.json → *.json-saved, etc.)
- Requires typing 'ARCHIVE' to confirm
- Won't archive if component is still in use
- Archived files are committed to git for recovery

## Component Registry Format

`config/component_registry.json`:
```json
{
  "components": {
    "goblin_left_ear": {
      "path": "config/bots/bot_families/goblins/head/goblin_left_ear.json",
      "registered": true
    }
  }
}
```

## Examples

### Registering a new component
```bash
$ python tools/register_component.py my_new_sensor
🔍 Validating component: my_new_sensor
✓ Found config/components/hardware/my_new_sensor.json
✓ Found config/components/hardware/my_new_sensor.src
✓ Found config/components/hardware/my_new_sensor.hdr
🔍 Validating component dependencies...
✓ All dependencies valid
✓ Registered my_new_sensor in component registry
✓ Committed my_new_sensor to repository

✅ Component my_new_sensor successfully registered and committed!
```

### Archiving a component
```bash
$ python tools/delete_component.py old_sensor
🔍 Searching for component: old_sensor
✓ Found config/components/hardware/old_sensor.json
✓ Found config/components/hardware/old_sensor.src
✓ Found config/components/hardware/old_sensor.hdr
🔍 Checking component usage...
✓ Component not referenced in any configurations

⚠️  WARNING: This will ARCHIVE the following files to config/delete-saved/:
  - config/components/hardware/old_sensor.json → old_sensor.json-saved
  - config/components/hardware/old_sensor.src → old_sensor.src-saved
  - config/components/hardware/old_sensor.hdr → old_sensor.hdr-saved

Original files will be removed from their current location.

Type 'ARCHIVE' to confirm: ARCHIVE
✓ Saved to config/delete-saved/old_sensor.json-saved
✓ Saved to config/delete-saved/old_sensor.src-saved
✓ Saved to config/delete-saved/old_sensor.hdr-saved
✓ Removed config/components/hardware/old_sensor.json from git
✓ Removed config/components/hardware/old_sensor.src from git
✓ Removed config/components/hardware/old_sensor.hdr from git
✓ Added saved files to git
✓ Removed old_sensor from component registry
✓ Committed changes to repository

✅ Component old_sensor archived to config/delete-saved/ and changes committed!
```

### Attempting to delete a component in use
```bash
$ python tools/delete_component.py goblin_left_eye
🔍 Searching for component: goblin_left_eye
✓ Found config/bots/bot_families/goblins/head/goblin_left_eye.json
✓ Found config/bots/bot_families/goblins/head/goblin_left_eye.src
✓ Found config/bots/bot_families/goblins/head/goblin_left_eye.hdr
🔍 Checking component usage...

❌ ERROR: Component goblin_left_eye is still referenced in:
  - config/bots/bot_families/goblins/head/goblin_head.json

Please remove these references first.
```

## Integration with Workflow

These tools should be used as part of your component development workflow:

1. **Create component**: Create .json, .src, .hdr files
2. **Register**: `python tools/register_component.py <name>`
3. **Use component**: Add to bot configuration
4. **Test**: Build and verify
5. **Remove from config**: Edit bot .json to remove component reference
6. **Delete**: `python tools/delete_component.py <name>`

## Notes

- Components with `"controller"` field don't require .src/.hdr files
- All three files (.json, .src, .hdr) are committed together
- Component registry maintains single source of truth for component paths
- Git operations are atomic - all files added/removed in single commit
