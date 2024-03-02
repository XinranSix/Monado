#include "sandbox/sandboxApp.h"

SandboxApp::SandboxApp() {}

SandboxApp::~SandboxApp() {};

Monado::Application *Monado::CreateApplication() { return new SandboxApp(); }
