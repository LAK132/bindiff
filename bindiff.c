#include "stdio.h"
#include "locale.h"
#include "wchar.h"

#define LINELENGTH 0x10

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "");
  fwide(stderr, 1);
  fwide(stdout, 1);

  if (argc < 3) {
    fwprintf(stderr, L"\x1B[31m\x1B[1m");
    fwprintf(stderr, L"Too few arguments\n");
    fwprintf(stderr, L"\x1B[0m");
    return 1;
  }

  FILE *file1 = NULL, *file2 = NULL;

  do {
    file1 = fopen(argv[1], "rb");
    file2 = fopen(argv[2], "rb");
    if (!file1 || !file2) {
      fwprintf(stderr, L"\x1B[31m\x1B[1m");
      fwprintf(stderr, L"Error opening file (%s %s)", file1 ? "" : argv[1], file2 ? "" : argv[2]);
      fwprintf(stderr, L"\x1B[0m");
      break;
    }
    unsigned char bytes1[LINELENGTH] = {0}, bytes2[LINELENGTH] = {0};
    size_t line = 0;
    do {
      size_t read1 = fread(bytes1, 1, LINELENGTH, file1);
      size_t read2 = fread(bytes2, 1, LINELENGTH, file2);
      size_t read = read1 > read2 ? read2 : read1;
      int diff = 0;
      for (size_t i = 0; i < read; ++i)
        if (bytes1[i] != bytes2[i])
          { diff = 1; break; }
      if (diff) {
        fwprintf(stdout, L"\n\x1B[0m%08zX \x1B[32m", line);
        for (size_t i = 0; i < read; ++i) {
          if (bytes1[i] != bytes2[i])
            fwprintf(stdout, L"\x1B[1m%02X ", bytes1[i]);
          else
            fwprintf(stdout, L"\x1B[2m%02X ", bytes1[i]);
        }
        fwprintf(stdout, L"\n\x1B[0m%08zX \x1B[31m", line);
        for (size_t i = 0; i < read; ++i) {
          if (bytes1[i] != bytes2[i])
            fwprintf(stdout, L"\x1B[1m%02X ", bytes2[i]);
          else
            fwprintf(stdout, L"\x1B[2m%02X ", bytes2[i]);
        }
        fwprintf(stdout, L"\n\x1B[0m");
      }
      if (ferror(file1) || ferror(file2)) {
        fwprintf(stderr, L"\x1B[31m\x1B[1m");
        fwprintf(stderr, L"File error (%i %i)", ferror(file1), ferror(file2));
        fwprintf(stderr, L"\x1B[0m");
        break;
      }
      line += LINELENGTH;
    } while (!feof(file1) && !feof(file2));
  } while(0);

  if (file1) fclose(file1);
  if (file2) fclose(file2);
}
