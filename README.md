SecureVault – Linux File & Password Security Tool
SecureVault is a Linux-based command-line security application developed in C++17. It provides a simple interface for protecting files through authentication, authenticated encryption, Linux file permissions, and activity logging.
1. Problem Statement
Sensitive files stored on a computer can be exposed if unauthorized users gain access to the system. File-system permissions alone do not provide complete protection for confidential data.
SecureVault addresses this problem by providing a local security tool that allows users to:
- Authenticate before accessing the application
- Add files to a protected vault
- Encrypt sensitive files
- Decrypt files using the correct encryption password
- View and modify Linux file permissions
- Maintain an activity log
2. Objectives
1. Develop a modular Linux security application using C++17.
2. Implement password-based authentication.
3. Protect files using AES-256-GCM authenticated encryption.
4. Derive encryption keys using PBKDF2-HMAC-SHA256.
5. Demonstrate Linux file-permission management using stat() and chmod().
6. Maintain an activity log for important application events.
7. Use GNU Make for compilation.
8. Apply Git/GitHub for version control.
9. Demonstrate practical integration of C++, Linux, and cybersecurity concepts.
3. Key Features
Authentication
- Master password setup during first-time use
- Master password verification
- Hidden terminal password input
File Management
- Add files to the protected vault
- List protected files
- Exclude .password from the normal vault listing
Encryption
- AES-256-GCM authenticated encryption
- PBKDF2-HMAC-SHA256 key derivation
- Random salt generation
- Random IV generation
- Authentication-tag verification
- .enc encrypted-file format
Linux Permissions
- View owner, group, and other permissions
- Change Linux permission modes such as 600, 644, and 700
Activity Logging
- Records authentication activity
- Records file operations
- Records encryption/decryption
- Records permission changes
- Records application startup/shutdown
4. System Architecture
                         USER
                           │
                           ▼
                  ┌─────────────────┐
                  │ SecureVault CLI  │
                  │    main.cpp      │
                  └────────┬────────┘
                           │
       ┌───────────────────┼───────────────────┐
       │                   │                   │
       ▼                   ▼                   ▼
┌──────────────┐   ┌──────────────┐   ┌────────────────┐
│Authentication│   │  Encryption  │   │ File Manager   │
│    Module    │   │    Module    │   │    Module      │
└──────┬───────┘   └──────┬───────┘   └───────┬────────┘
       │                  │                    │
       ▼                  ▼                    ▼
 Password             OpenSSL             std::filesystem
 Verification         AES-256-GCM          Protected vault
                      PBKDF2-SHA256
                           │
             ┌─────────────┴─────────────┐
             ▼                           ▼
     ┌───────────────┐           ┌───────────────┐
     │  Permissions  │           │    Logger     │
     │ stat/chmod    │           │ activity.log  │
     └───────────────┘           └───────────────┘
5. Project Structure
SecureVault/
├── Makefile
├── README.md
├── .gitignore
├── include/
│   ├── authentication.h
│   ├── encryption.h
│   ├── file_manager.h
│   ├── logger.h
│   └── permissions.h
├── src/
│   ├── authentication.cpp
│   ├── encryption.cpp
│   ├── file_manager.cpp
│   ├── logger.cpp
│   ├── main.cpp
│   └── permissions.cpp
├── tests/
├── logs/
└── vault/
Runtime data such as the vault contents, logs, and compiled executable are excluded from Git through .gitignore.
6. Technology Stack
Component	Technology
Language	C++17
OS	Linux / Ubuntu
Development Environment	WSL2
Compiler	g++
Build System	GNU Make
Cryptographic Library	OpenSSL
Encryption	AES-256-GCM
Key Derivation	PBKDF2-HMAC-SHA256
File Operations	C++17 std::filesystem
Permissions	Linux stat() / chmod()
Version Control	Git
Repository Hosting	GitHub


7. Module Description
Main Controller
File: src/main.cpp
Responsible for:
- Starting SecureVault
- Authenticating the user
- Displaying the menu
- Handling user choices
- Calling the required modules
- Exiting the application
Authentication Module
Files: include/authentication.h, src/authentication.cpp
Responsible for:
- Checking whether a master password exists
- Creating the master password
- Verifying the master password
- Hashing the master password using SHA-256
- Hiding password input using Linux terminal controls
Security limitation: The current master-password mechanism uses a locally stored SHA-256 hash. This is acceptable for the academic prototype but is not production-grade password storage. A password-specific algorithm such as Argon2, bcrypt, or scrypt would be a future improvement.
Encryption Module
Files: include/encryption.h, src/encryption.cpp
Responsible for:
- Deriving encryption keys
- Encrypting files
- Decrypting files
- Generating random salt and IV values
- Verifying AES-GCM authentication tags
File Manager
Files: include/file_manager.h, src/file_manager.cpp
Responsible for:
- Adding files to vault/
- Validating source files
- Listing protected files
- Excluding .password from normal vault listings
Permissions Module
Files: include/permissions.h, src/permissions.cpp
Responsible for:
- Viewing Linux file permissions
- Changing permissions with chmod()
Logger Module
Files: include/logger.h, src/logger.cpp
Responsible for maintaining the application activity log.
8. Encryption Design
SecureVault uses AES-256-GCM for authenticated file encryption.
Parameter	Value
Algorithm	AES-256-GCM
Key Size	32 bytes / 256 bits
Salt Size	16 bytes
IV Size	12 bytes
Authentication Tag	16 bytes
PBKDF2 Iterations	100,000
KDF Hash	SHA-256


Key Derivation
Encryption Password
        │
        ▼
PBKDF2-HMAC-SHA256
        │
        ├── Random 16-byte Salt
        └── 100,000 iterations
        │
        ▼
256-bit AES Key
Encryption Workflow
Select file
    │
    ▼
Read plaintext
    │
    ▼
Generate random salt + IV
    │
    ▼
Derive AES-256 key
    │
    ▼
AES-256-GCM encryption
    │
    ├── Ciphertext
    └── Authentication Tag
    │
    ▼
Write encrypted file
Encrypted File Format
┌────────────┬────────────┬────────────────┬──────────────┐
│    SALT    │     IV     │   CIPHERTEXT   │     TAG      │
│  16 bytes  │  12 bytes  │ Variable size  │  16 bytes    │
└────────────┴────────────┴────────────────┴──────────────┘
The salt and IV do not need to be secret. The password is required to derive the correct encryption key.
Decryption Workflow
Encrypted file
      │
      ▼
Read Salt + IV + Ciphertext + Tag
      │
      ▼
Derive key from supplied password
      │
      ▼
AES-256-GCM authentication
      │
      ├── Valid tag ──────► Write plaintext
      │
      └── Invalid tag ────► Reject decryption
A wrong password or corrupted encrypted file causes authentication-tag verification to fail. Plaintext is written only after successful finalization.
9. Linux File Permissions
SecureVault uses Linux stat() to inspect permissions and chmod() to modify them.
Permissions are displayed for:
- Owner
- Group
- Others
Where:
- R = Read
- W = Write
- X = Execute
- - = Permission not granted
Examples:
- 600 → Owner read/write only
- 644 → Owner read/write, others read
- 700 → Owner full access
10. Activity Logging
Important application events are recorded in:
logs/activity.log
Examples include:
- Application startup
- Successful authentication
- Failed authentication
- File added
- File encrypted
- File decrypted
- Permission changed
- Application shutdown
11. Build and Installation
Prerequisites
Ubuntu/Linux environment with:
- g++
- GNU Make
- OpenSSL development libraries
Install dependencies:
sudo apt update
sudo apt install build-essential libssl-dev openssl
Build
cd ~/SecureVault
make
Run
./securevault
Clean Build
make clean
12. Application Menu
========================================
                 MENU
========================================
1. Add File
2. List Protected Files
3. Encrypt File
4. Decrypt File
5. View File Permissions
6. Change File Permissions
7. View Activity Log
8. Exit
13. Typical User Workflow
Start Application
       │
       ▼
Check Master Password
       │
       ├── Not Found ──► Create Password
       │
       ▼
Authenticate
       │
       ├── Failed ─────► Access Denied
       │
       ▼
Main Menu
       │
       ├── Add File
       ├── List Files
       ├── Encrypt
       ├── Decrypt
       ├── View Permissions
       ├── Change Permissions
       └── View Activity Log
14. Testing
The final testing process covers:
Test	Expected Result
First-time password creation	Password created successfully
Correct master password	Authentication succeeds
Incorrect master password	Access denied
Add valid file	File copied to vault
Add invalid path	Operation rejected
List vault	Protected files displayed
Encrypt valid file	.enc file created
Encrypt .enc file	Operation rejected
Decrypt with wrong password	Decryption fails
Decrypt with correct password	Original file recreated
View permissions	Linux permissions displayed
Change permissions	Permissions updated
View activity log	Logged events displayed
Invalid menu option	Error handled without crash


15. Development Stages
The project follows the six-stage training structure.
Stage 1 – Project Introduction
- Project idea
- Problem identification
- Objectives
- Scope
- Expected outcome
Stage 2 – Requirements & Development Plan
- Functional requirements
- Non-functional requirements
- Module identification
- Development timeline
- Deliverables
Stage 3 – System Design & Architecture
- System architecture
- Module responsibilities
- Data flow
- UML diagrams
- Development environment
- Git/version-control plan
Stage 4 – Initial Implementation & Prototype
- Authentication
- File management
- Encryption/decryption
- Permissions
- Logging
- Module integration
Stage 5 – Testing, Integration & Improvement
- Functional testing
- Integration testing
- Encryption/decryption testing
- Incorrect-password testing
- Permission testing
- Error handling
- Code refinement
Stage 6 – Final Implementation & Presentation
- Final working system
- Testing results
- Documentation
- GitHub repository
- Demonstration
- Limitations
- Future improvements
16. Limitations
The current academic prototype has several limitations:
1. The master-password hash uses SHA-256 rather than a dedicated password-hashing algorithm.
2. The application is command-line based.
3. There is no multi-user account system.
4. There is no cloud backup or remote storage.
5. Passwords are kept in application memory while being used.
6. The project is primarily designed for Linux environments.
17. Future Improvements
Potential improvements include:
- Argon2/bcrypt/scrypt for master-password storage
- Secure password input with additional memory handling
- Secure key management
- Multi-user support
- Graphical user interface
- File-integrity monitoring
- Automatic encrypted backups
- Secure cloud storage integration
- Hardware-backed key protection
- Improved audit logging
18. Learning Outcomes
Through SecureVault, the project demonstrates practical understanding of:
- C++17 programming
- Object-oriented modular design
- Linux file systems
- Linux file permissions
- System-level APIs
- Cryptographic primitives
- AES-GCM authenticated encryption
- PBKDF2 key derivation
- OpenSSL EVP APIs
- GNU Make
- Error handling
- Git/GitHub
- Software testing
- Security-oriented application design
19. Conclusion
SecureVault demonstrates how C++ and Linux system functionality can be combined to build a practical file-security application.
The project integrates authentication, protected file management, AES-256-GCM encryption, PBKDF2-based key derivation, Linux permission management, and activity logging into a single command-line application.
The resulting system provides a practical academic demonstration of software development, Linux programming, cryptography, security, testing, and version control.
