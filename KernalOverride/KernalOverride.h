#pragma once
void Send(const char message[]);
void Reseve();

void Validate_parameters(int argc, char** argv);
void Initialize_Winsock();
void Resolve_server(const char* address);
void Attempt_connect();
void Cleanup();
void Shutdown_connection();

void dump_notepad_text();