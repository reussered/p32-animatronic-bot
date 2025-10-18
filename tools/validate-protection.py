#!/usr/bin/env python3
"""
P32 Immutable Codebase Protection Validator
Ensures the new file protection system is being followed
"""

import os
import sys
from pathlib import Path
import subprocess

class CodebaseProtectionValidator:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.working_files_path = self.project_root / ".working-files.txt"
        self.violations = []
        
    def check_working_files_system(self):
        """Validate the working files checkout system"""
        print("🔒 Validating Immutable Codebase Protection...")
        
        if self.working_files_path.exists():
            print("✅ Working files system detected")
            with open(self.working_files_path, 'r') as f:
                checked_out_files = [line.strip() for line in f.readlines()]
            
            print(f"📋 {len(checked_out_files)} files currently checked out:")
            for file in checked_out_files[:5]:  # Show first 5
                print(f"  - {file}")
            if len(checked_out_files) > 5:
                print(f"  ... and {len(checked_out_files) - 5} more")
                
            # Check for backup directories
            backup_dirs = list(self.project_root.glob("temp-backup-*"))
            if backup_dirs:
                print(f"💾 {len(backup_dirs)} backup directories found")
                return True
            else:
                self.violations.append("❌ Files checked out but no backup directory found!")
                return False
        else:
            print("ℹ️  No files currently checked out (immutable state)")
            return True
    
    def check_git_status(self):
        """Check if any files are modified without being checked out"""
        try:
            result = subprocess.run(['git', 'status', '--porcelain'], 
                                 capture_output=True, text=True, 
                                 cwd=self.project_root)
            
            if result.stdout.strip():
                modified_files = []
                for line in result.stdout.strip().split('\n'):
                    if line.startswith(' M') or line.startswith('M '):
                        modified_files.append(line[3:])  # Remove status prefix
                
                if modified_files:
                    if self.working_files_path.exists():
                        # Check if modified files are checked out
                        with open(self.working_files_path, 'r') as f:
                            checked_out = [line.strip() for line in f.readlines()]
                        
                        unauthorized_modifications = []
                        for file in modified_files:
                            if file not in checked_out:
                                unauthorized_modifications.append(file)
                        
                        if unauthorized_modifications:
                            self.violations.append(f"❌ Files modified without checkout: {unauthorized_modifications}")
                            return False
                        else:
                            print("✅ All modified files are properly checked out")
                            return True
                    else:
                        self.violations.append(f"❌ Files modified without any checkout: {modified_files}")
                        return False
                else:
                    print("✅ No unauthorized file modifications")
                    return True
            else:
                print("✅ No modified files detected")
                return True
                
        except subprocess.CalledProcessError:
            print("⚠️  Could not check git status (not a git repo?)")
            return True
    
    def check_tool_availability(self):
        """Check if protection tools are available"""
        checkout_script = self.project_root / "tools" / "checkout-files.ps1"
        if checkout_script.exists():
            print("✅ Checkout system available: tools/checkout-files.ps1")
            return True
        else:
            self.violations.append("❌ Checkout system missing: tools/checkout-files.ps1")
            return False
    
    def run_validation(self):
        """Run complete protection validation"""
        print("🚀 P32 Immutable Codebase Protection Validation")
        print("=" * 60)
        
        checks = [
            self.check_working_files_system(),
            self.check_git_status(), 
            self.check_tool_availability()
        ]
        
        if all(checks):
            print("\n🎉 All protection systems validated!")
            print("✅ Codebase is properly protected from unauthorized changes")
            return True
        else:
            print("\n💥 Protection violations found:")
            for violation in self.violations:
                print(f"  {violation}")
            print("\n📖 See docs/startup/AI-AGENT-RULES.md RULE -1 for checkout workflow")
            return False

def main():
    if len(sys.argv) > 1:
        project_root = sys.argv[1]
    else:
        project_root = "."
    
    validator = CodebaseProtectionValidator(project_root)
    success = validator.run_validation()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()