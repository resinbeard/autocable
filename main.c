/*
    autocable
    Loads a file of specified port configurations and connects them.
    Intended to use with the Jack Audio Connection Kit daemon.

    Copyright (C) 2010 Murray Foster

    Please forward questions to mrafoster@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <jack/jack.h>

/* string buffer size */
#define MAX_LEN 300

void
show_version (char *my_name)
{
	printf ("\n\n%s 1.0\n", my_name);
}

void
show_usage (char *my_name)
{
	show_version (my_name);
	fprintf (stderr, "\nusage: %s directory/file.ac\n", my_name);
	fprintf (stderr, "Eats your text files and routes Jack connections for you.\n\n");
	fprintf (stderr, "       In your file, use full port names and return each connection.\n");
    fprintf (stderr, "          ie. system:capture_1 system:playback_1\n");
    fprintf (stderr, "              system:capture_2 system:playback_2\n\n");
}

int main (int argc, char *argv[])
{
    FILE *input_file;
    char *file_directory = NULL;
    char string_buffer[MAX_LEN + 1];

	jack_client_t *client;
	jack_status_t status;
    char *server_name = NULL;
	jack_options_t options = JackNoStartServer;

	char *my_name = "autocable";

	char *action = NULL;
	char *src_name = NULL;
	char *dst_name = NULL;
	jack_port_t *src_port = 0;
	jack_port_t *dst_port = 0;
	int no_spec = 0;

	if ( (argc < 1) || (argc > 2) ) 
	  no_spec = 1;

	/* try to become a client of the JACK server */

	if ((client = jack_client_open (my_name, options, &status, server_name)) == 0) {
		fprintf (stderr, "jack server not running?\n");
		return 1;
	}

	/* open user-defined directory */

	if (argc == 2)
	  if ( (file_directory = argv[1]) == 0 && (no_spec == 1))
	    {
	      show_usage(my_name);
	      return 1;
	    }
	  else
	    {
	      /* let's try to open the file... */    
	      input_file = fopen(file_directory, "r");
	      
	      if(input_file != NULL)
		printf("\nautocable has eaten your file and is getting to work!\n\n");
	      else
		{
		  fprintf (stderr, "ERROR %s not a valid directory\n", argv[1]);
		  show_usage(my_name);
		  return 1;
		}
	    }
	else
	  input_file = stdin;
        /* main function loop */

        while(fgets(string_buffer, MAX_LEN, input_file) != NULL)
        {
	  if( strcmp(string_buffer,"\n") ) { 
            /* break up the line read from the file */
	    action = strtok(string_buffer, " ");
            src_name = strtok(NULL, " ");
            dst_name = strtok(NULL, "\n");
            printf("attempting to %s %s and %s..\n", action, src_name, dst_name);
	    /* make port connection */
	    if( !strcmp("connect",action) )
	      jack_connect(client, src_name, dst_name);
	    else if( !strcmp("disconnect",action) )
	      jack_disconnect(client, src_name, dst_name);
	    else {
	      fprintf(stderr, "unknown command '%s'!\n", action);
	      return 1;
	    }
	  }
        }
        printf("\nfinished!\n\n");
    

    /* close shit down! */
	jack_client_close (client);
    fclose(input_file);
	return 0;
}

