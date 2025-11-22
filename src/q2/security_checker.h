#ifndef SECURITY_CHECKER_H
#define SECURITY_CHECKER_H

void run_scan_mode(char **files, int file_count);
void run_recommendation_mode(char **files, int file_count);
void run_extended_mode(char **files, int file_count);

#endif // SECURITY_CHECKER_H
