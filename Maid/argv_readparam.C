/*BEGIN_LEGAL 
INTEL CONFIDENTIAL
Copyright 2002-2005 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents
related to the source code (Material) are owned by Intel Corporation
or its suppliers or licensors. Title to the Material remains with
Intel Corporation or its suppliers and licensors. The Material may
contain trade secrets and proprietary and confidential information of
Intel Corporation and its suppliers and licensors, and is protected by
worldwide copyright and trade secret laws and treaty provisions. No
part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in
any way without Intels prior express written permission.  No license
under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or
delivery of the Materials, either expressly, by implication,
inducement, estoppel or otherwise. Any license under such intellectual
property rights must be express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or
alter this notice or any other notice embedded in Materials by Intel
or Intels suppliers or licensors in any way.
END_LEGAL */
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argv_readparam.h"

/*
 * Example: tar -xzvf foo.tar.gz foo/
 * Rules: * Must begin with -
 *        * One or more options is ok.  (i.e. -x -z == -xz)
 *
 */
int 
argv_hasFlag(int argc, char *argv[], char param)
{
  int i;

  if( argc == 0
      || argv == NULL ) {
    return 0;
  }

  for(i = 0; i < argc; i++) {
    if( strlen(argv[i]) >= 2
	&& argv[i][0] == '-'
	&& argv[i][1] != '-'
	&&  strchr(argv[i], param) ) {
      return 1;
    }
  }
  
  return 0;   
}

/*
 * Example: ./foo --verbose
 * Rules: * Must begin with --
 *        * Only one option per --
 * 
 */
int 
argv_hasLongFlag(int argc, char *argv[], char *param)
{
  int i;

  if( argc == 0
      || argv == NULL
      || param == NULL ) {
    return 0;
  }

  for( i = 0; i < argc; i++ ) {
    if( strcmp(param, argv[i]) == 0 ) {
      return 1;      
    }
  }
  
  return 0;
}

/*
 * Examples: ./foo -b=5
 *           ./foo --bar=5
 *           ./foo -D7
 *           ./foo foo=7
 * 
 */
int 
argv_getInt(int argc, char *argv[], char *param, int *ret)
{
  int i;
  int fmt_len;
  char *fmt;
  
  if( argc == 0
      || argv == NULL 
      || param == NULL
      || ret == NULL ) {
    return 0;
  }
  
  fmt_len = strlen(param) + 2;
  if( !(fmt = (char*)malloc(fmt_len)) ) {
    perror("argv_getInt");
    return 0;
  }
  
  if( strcpy(fmt, param) == NULL ) {
    perror("argv_getInt");
    return 0;
  }
  
  fmt[fmt_len - 2] = '%';
  fmt[fmt_len - 1] = 'd';  
  
  for( i = argc - 1; i >= 0; i-- ) {
    if( sscanf(argv[i], fmt, ret) > 0 ) {
      return 1;
    }
  }
  
  free(fmt);
  return 0;    
}

/*
 * Examples: ./foo -b=5
 *           ./foo --bar=5
 *           ./foo -D7
 *           ./foo foo=7
 * 
 */
int 
argv_getLong(int argc, char** argv, char *param, long *ret)
{
  int i;
  int fmt_len;
  char *fmt;

  if( argc == 0
      || argv == NULL 
      || param == NULL
      || ret == NULL ) {
    return 0;
  }
  
  fmt_len = strlen(param) + 3;
  if( !(fmt = (char*)malloc(sizeof(char) * fmt_len)) ) {
    perror("argv_getLong");
    return 0;
  }
  
  if( strcpy(fmt, param) == NULL ) {
    perror("argv_getLong");
    return 0;
  }

  fmt[fmt_len - 3] = '%';
  fmt[fmt_len - 2] = 'l';
  fmt[fmt_len - 1] = 'd';  

  for( i = argc - 1; i >= 0; i-- ) {
    if( sscanf(argv[i], fmt, ret) > 0 ) {
      return 1;
    }
  }
  
  free(fmt);
  return 0;
}


/*
 * Examples: ./foo -b=foo
 *           ./foo --bar=baz
 *           ./foo -DHAVE_CONFIG
 *           ./foo foo=5
 *           ./foo --name="Tipp Moseley"
 * 
 */
char *
argv_getString(int argc, char *argv[], char *param, char **mem)
{
  int i;
  int paramlen;
  char *ret;

  if( argc == 0
      || argv == NULL 
      || param == NULL 
      || (mem != NULL && *mem == NULL) ) {
    return NULL;
  }
  
  if( (paramlen = strlen(param)) <= 0 ) {
    return NULL;
  }

  for( i = argc - 1; i >= 0; i-- ) {
    if( strncmp(param, argv[i], paramlen) == 0 ) {
      if( mem == NULL ) {
	ret = (char*)malloc(sizeof(char) * (strlen(argv[i]) - paramlen + 1));
	if( ret == NULL ) {
	  perror("argv_getString");
	  return NULL;
	}
      } else {
	ret = *mem;
      }
      
      if( argv[i][paramlen] == '"'
	  && argv[i][strlen(argv[i]) - 1] == '"' ) {
	return strncpy(ret, &argv[i][paramlen + 1], 
		       strlen(argv[i]) - paramlen - 2);
      } else {
	return strcpy(ret, &argv[i][paramlen]);
      }
    }
  }
  
  return NULL;
}

/*
int main(int argc, char **argv) 
{
#define argc1 6
  int iret;
  long lret;
  char *cret;

  char *argv1[argc1+1] = {
    "--foo",
    "-f",
    "-DFOO",
    "--name=\"tipp moseley\"",
    "--empty=\"\"",
    "-N=42",
    NULL
  };

  if( !argv_hasLongFlag(argc1, argv1, "--foo") ) {
    printf("ERROR 1");
  }

  if( !argv_hasFlag(argc1, argv1, 'f') ) {
    printf("ERROR 2");
  }

  if( !argv_getInt(argc1, argv1, "-N=", &iret) ) {
    printf("ERROR 3");
  } else {
    printf("-N=%d\n", iret);
  }

  if( !argv_getLong(argc1, argv1, "-N=", &lret) ) {
    printf("ERROR 4");
  } else {
    printf("-N=%ld\n", lret);
  }
  
  if( !(cret = argv_getString(argc1, argv1, "-D", NULL)) ) {
    printf("ERROR 5");
  } else {
    printf("-D%s\n", cret);
  }

  if( !(cret = argv_getString(argc1, argv1, "--name=", NULL)) ) {
    printf("ERROR 6");
  } else {
    printf("--name=%s\n", cret);
  }

  if( !(cret = argv_getString(argc1, argv1, "--empty=", NULL)) ) {
    printf("ERROR 7");
  } else {
    printf("--empty=%s\n", cret);
  }
  
  return 0;
}
*/
