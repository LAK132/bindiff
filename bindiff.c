#include "stdio.h"
#include "stdlib.h"
#include "locale.h"
#include "wchar.h"

#define LINELENGTH 0x10

FILE *file[] = {NULL, NULL};

void finish(int status)
{
  if (file[0]) fclose(file[0]);
  if (file[1]) fclose(file[1]);
  fwprintf(stdout, L"\x1B[0m");
  exit(status);
}

void open_file(int file_number, const char *path)
{
  if (!(file[file_number] = fopen(path, "rb")))
  {
    fwprintf(stderr,
             L"\x1B[31m\x1B[1mError opening file %i: \"%s\"\x1B[0m\n",
             file_number + 1,
             path);
    finish(EXIT_FAILURE);
  }
}

void check_file_errors(int file_number)
{
  int err = ferror(file[file_number]);
  if (err)
  {
    fwprintf(stderr,
             L"\x1B[31m\x1B[1mError reading file %i: %i\x1B[0m\n",
             file_number + 1,
             err);
    finish(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "");
  fwide(stderr, 1);
  fwide(stdout, 1);

  // Reset the graphics mode before we begin.
  fwprintf(stdout, L"\x1B[0m");

  if (argc < 3)
  {
    fwprintf(stderr, L"\x1B[31m\x1B[1mToo few arguments\x1B[0m\n");
    finish(EXIT_FAILURE);
  }

  open_file(0, argv[1]);
  open_file(1, argv[2]);

  fwprintf(stdout, L"\x1B[0mA: \x1B[32m%s\n", argv[1]);
  fwprintf(stdout, L"\x1B[0mB: \x1B[31m%s\n", argv[2]);

  unsigned char bytes1[LINELENGTH] = {0};
  unsigned char bytes2[LINELENGTH] = {0};

  for (size_t line = 0; !feof(file[0]) && !feof(file[1]); line += LINELENGTH)
  {
    size_t read1 = fread(bytes1, 1, LINELENGTH, file[0]);
    size_t read2 = fread(bytes2, 1, LINELENGTH, file[1]);
    size_t read = read1 > read2 ? read2 : read1;
    int diff = 0;

    for (size_t i = 0; i < read; ++i)
    {
      if (bytes1[i] != bytes2[i])
      {
        diff = 1;
        break;
      }
    }

    if (diff)
    {
      // Print green line (file 1).
      fwprintf(stdout, L"\n\x1B[0m%08zX", line);

      for (size_t i = 0; i < read; ++i)
      {
        fwprintf(stdout, L" \x1B[0m\x1B[32m");
        if (bytes1[i] != bytes2[i])
          fwprintf(stdout, L"\x1B[1m%02X", bytes1[i]);
        else
          fwprintf(stdout, L"\x1B[2m%02X", bytes1[i]);
      }

      // Print red line (file 2).
      fwprintf(stdout, L"\n\x1B[0m%08zX", line);

      for (size_t i = 0; i < read; ++i)
      {
        fwprintf(stdout, L" \x1B[0m\x1B[31m");
        if (bytes1[i] != bytes2[i])
          fwprintf(stdout, L"\x1B[1m%02X", bytes2[i]);
        else
          fwprintf(stdout, L"\x1B[2m%02X", bytes2[i]);
      }

      fwprintf(stdout, L"\n");
    }

    check_file_errors(0);
    check_file_errors(1);
  }

  finish(EXIT_SUCCESS);
}
