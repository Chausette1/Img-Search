
#ifndef PROJOS_DIG
#define PROJOS_SIG

void handler_sigint(int signal_recu);
void hide_sigterm();
void unhide_sigterm();
void handler_sigterm(int signal_recu);

#endif