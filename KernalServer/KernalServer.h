#pragma once
void Send(const char message[]);
void Reseve();

void Initialize_Winsock();
void Resolve_server();
void Create_SOCKET();
void Setup_TCP();
void Accept_client();
void Shutdown_connection();