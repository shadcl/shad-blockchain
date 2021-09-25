#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

char*
shad_path_join (int argc, ...)
{
  va_list valist;
  char *path;
  int i;
  int size;

  va_start(valist, argc);

  for (i = 0; i < argc; i++)
    {
      size += strlen (va_arg(valist, char*));
    }
  path = calloc (1, (size + argc));

  va_start(valist, argc);

  for (i = 0; i < argc; i++)
    {
      strcat (path, "/");
      strcat (path, va_arg(valist, char*));
    }

  va_end(valist);

  return path;
}

void
shad_path_free (char *path)
{
  free (path);
}
