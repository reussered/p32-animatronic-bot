# CRITICAL DEVELOPMENT RULES - READ FIRST ALWAYS

## Environment Requirements
- Windows PowerShell environment (powershell.exe v5.1)
- ASCII encoding ONLY - NO UNICODE ANYWHERE
- Windows NTFS file system with backslash paths
- CRLF line endings (not LF)
- Current directory: F:\GitHub\p32-animatronic-bot

## Mandatory Development Practices
1. Use Windows PowerShell syntax only
2. Use backslash paths: F:\GitHub\p32-animatronic-bot\config\file.json
3. NO Linux/Unix commands ever
4. Create files that work natively on Windows
5. Use simple PowerShell commands, not complex scripts
6. ASCII encoding only - never Unicode
7. Test before scaling up
8. One file at a time if complex operations fail

## Token Cost Awareness
- Failed commands cost tokens from user budget
- Repeated mistakes are expensive
- Test approach before implementing at scale
- Use proven patterns that work

## Current Project Status
- Reduced from 1000+ errors to 54 critical problems
- Two successful major commits completed
- 26 missing goblin component files remain
- Interface library established and working