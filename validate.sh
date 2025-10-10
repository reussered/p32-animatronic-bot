#!/bin/bash
# Convenience script to validate all JSON files in the assets folder

echo "==================================================="
echo "  P32 Animatronic Bot - JSON Validation"
echo "==================================================="
echo ""

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Run the Python validator
python3 "$SCRIPT_DIR/validate_json.py" "$@"

exit $?
