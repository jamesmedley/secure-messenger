# Secure Messenger App

A secure messaging application written in C. Its interactive GUI provides end-to-end encryption and AI-driven phishing detection for safer communication.

# Features
- Custom TLS-like handshake and key exchange
- Custom protocol for message encoding 
- RSA/AES end-to-end encryption
    - RSA for key exchange and digital signatures
    - AES-256-CBC for message confidentiality and integrity
- AI phishing detection
    - Local model inference to flag suspicious links/messages
- GUI with [GTK](https://gitlab.gnome.org/GNOME/gtk)
- Focus on privacy: minimal telemetry, no server-side message storage
- Limited to local network