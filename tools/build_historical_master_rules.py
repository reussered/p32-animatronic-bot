"""
Build master_rules.json from git history.
Walks back through recent commits, captures file lists, and merges them into a comprehensive truth database.
"""
import json
import os
import shutil
import subprocess
from pathlib import Path
from datetime import datetime
from collections import defaultdict

class HistoricalMasterRulesBuilder:
    def __init__(self, num_commits=20):
        self.root_dir = Path(__file__).parent.parent.resolve()
        self.backup_dir = self.root_dir.parent / f"backup_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        self.num_commits = num_commits
        self.all_files_seen = set()  # Union of all files across all commits
        self.commit_history = []  # List of (commit_hash, commit_date, files_in_commit)
        
    def backup_workspace(self):
        """Backup current workspace state."""
        print(f"[BACKUP] Creating backup at {self.backup_dir}...")
        self.backup_dir.mkdir(exist_ok=True)
        
        # Backup config/ and assets/ directories
        config_src = self.root_dir / "config"
        config_dst = self.backup_dir / "config"
        if config_src.exists():
            shutil.copytree(config_src, config_dst)
            print(f"  -> Backed up config/")
        
        assets_src = self.root_dir / "assets"
        assets_dst = self.backup_dir / "assets"
        if assets_src.exists():
            shutil.copytree(assets_src, assets_dst)
            print(f"  -> Backed up assets/")
        
        # Backup master_rules.json
        master_rules_src = self.root_dir / "master_rules.json"
        if master_rules_src.exists():
            shutil.copy2(master_rules_src, self.backup_dir / "master_rules.json")
            print(f"  -> Backed up master_rules.json")
        
        print(f"[OK] Backup complete at {self.backup_dir}")
        return self.backup_dir
    
    def get_recent_commits(self):
        """Get list of recent commits from git."""
        print(f"\n[GIT] Fetching {self.num_commits} recent commits...")
        try:
            result = subprocess.run(
                ["git", "log", f"--oneline", f"-{self.num_commits}"],
                cwd=self.root_dir,
                capture_output=True,
                text=True
            )
            commits = []
            for line in result.stdout.strip().split('\n'):
                if line.strip():
                    parts = line.split(' ', 1)
                    commit_hash = parts[0]
                    message = parts[1] if len(parts) > 1 else ""
                    commits.append((commit_hash, message))
            
            print(f"  -> Found {len(commits)} commits")
            return commits
        except Exception as e:
            print(f"[ERROR] Failed to get commits: {e}")
            return []
    
    def get_files_at_commit(self, commit_hash):
        """Get list of all .json files at a specific commit."""
        try:
            result = subprocess.run(
                ["git", "ls-tree", "-r", "--name-only", commit_hash],
                cwd=self.root_dir,
                capture_output=True,
                text=True
            )
            
            files = []
            for line in result.stdout.strip().split('\n'):
                if line.endswith('.json'):
                    files.append(line)
            
            return files
        except Exception as e:
            print(f"[ERROR] Failed to get files at {commit_hash}: {e}")
            return []
    
    def get_commit_date(self, commit_hash):
        """Get commit date."""
        try:
            result = subprocess.run(
                ["git", "log", "-1", "--format=%aI", commit_hash],
                cwd=self.root_dir,
                capture_output=True,
                text=True
            )
            return result.stdout.strip()
        except:
            return "unknown"
    
    def checkout_commit(self, commit_hash):
        """Checkout a specific commit."""
        try:
            # Stash any uncommitted changes first
            subprocess.run(
                ["git", "stash"],
                cwd=self.root_dir,
                capture_output=True
            )
            
            subprocess.run(
                ["git", "checkout", commit_hash],
                cwd=self.root_dir,
                capture_output=True,
                check=True
            )
            return True
        except Exception as e:
            print(f"[ERROR] Failed to checkout {commit_hash}: {e}")
            return False
    
    def checkout_branch(self, branch="main"):
        """Return to a branch."""
        try:
            subprocess.run(
                ["git", "checkout", branch],
                cwd=self.root_dir,
                capture_output=True,
                check=True
            )
            
            # Pop stashed changes
            subprocess.run(
                ["git", "stash", "pop"],
                cwd=self.root_dir,
                capture_output=True
            )
            
            return True
        except Exception as e:
            print(f"[ERROR] Failed to checkout {branch}: {e}")
            return False
    
    def walk_history(self):
        """Walk through git history and collect file lists."""
        commits = self.get_recent_commits()
        
        # Reverse so we go from oldest to newest
        commits.reverse()
        
        print(f"\n[WALK] Walking through {len(commits)} commits (oldest to newest)...\n")
        
        for i, (commit_hash, message) in enumerate(commits, 1):
            print(f"[{i}/{len(commits)}] Checking out {commit_hash[:8]} - {message[:50]}")
            
            if not self.checkout_commit(commit_hash):
                continue
            
            # Get files at this commit
            files = self.get_files_at_commit(commit_hash)
            commit_date = self.get_commit_date(commit_hash)
            
            print(f"       -> Found {len(files)} .json files")
            
            # Add to history
            self.commit_history.append({
                'commit': commit_hash,
                'date': commit_date,
                'message': message,
                'file_count': len(files)
            })
            
            # Track all files
            self.all_files_seen.update(files)
        
        # Return to main branch
        print(f"\n[RESTORE] Returning to main branch...")
        self.checkout_branch("main")
    
    def build_comprehensive_master_rules(self):
        """Build a master_rules that includes all files ever seen."""
        print(f"\n[BUILD] Building comprehensive historical_master_rules.json...")
        print(f"  -> Total unique files seen across all commits: {len(self.all_files_seen)}")
        
        # Save to SEPARATE file: historical_master_rules.json (NOT the main one!)
        historical_rules_path = self.root_dir / "historical_master_rules.json"
        
        print(f"  -> IMPORTANT: Saving to separate file: {historical_rules_path}")
        print(f"  -> Main master_rules.json will NOT be modified")
        
        # Create fresh historical master_rules (independent from current one)
        historical_master_rules = {
            "schema_version": "1.2",
            "description": "Historical master_rules built from git history. Shows all files that EVER existed in the project.",
            "project_files": {
                "description": "Master list of all component JSON files that have ever existed in the project.",
                "files": []
            },
            "type_definitions": {
                "types": {}
            },
            "git_history": {
                "description": "Git history used to build this master_rules",
                "commits": []
            }
        }
        
        # Sort files and add to historical master_rules
        all_files_sorted = sorted(list(self.all_files_seen))
        historical_master_rules["project_files"]["files"] = all_files_sorted
        historical_master_rules["git_history"]["commits"] = self.commit_history
        
        print(f"  -> Added {len(all_files_sorted)} files to historical master_rules")
        
        # Save SEPARATE historical master_rules
        with open(historical_rules_path, 'w', encoding='utf-8') as f:
            json.dump(historical_master_rules, f, indent=4)
        
        print(f"[OK] Historical master rules saved to {historical_rules_path}")
        
        # Print summary
        print(f"\n[SUMMARY]")
        print(f"  - Commits walked: {len(self.commit_history)}")
        print(f"  - Unique files seen: {len(self.all_files_seen)}")
        print(f"  - Backup location: {self.backup_dir}")
        print(f"  - Historical master_rules location: {historical_rules_path}")
    
    def run(self):
        """Run the full workflow."""
        print("="*60)
        print("HISTORICAL MASTER RULES BUILDER")
        print("="*60)
        
        # Backup workspace
        self.backup_workspace()
        
        # Walk git history
        self.walk_history()
        
        # Build comprehensive master_rules
        self.build_comprehensive_master_rules()
        
        print("\n[SUCCESS] Historical master rules built!")
        print(f"Backup saved to: {self.backup_dir}")
        print(f"To restore, copy files from {self.backup_dir} back to {self.root_dir}")

if __name__ == "__main__":
    builder = HistoricalMasterRulesBuilder(num_commits=20)
    builder.run()
