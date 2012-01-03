//
//  MrsWatson.c
//  MrsWatson
//
//  Created by Nik Reiman on 12/5/11.
//  Copyright (c) 2011 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EventLogger.h"
#include "CharString.h"
#include "MrsWatson.h"
#include "BuildInfo.h"
#include "ProgramOption.h"
#include "InputSource.h"

CharString getNewVersionString(void) {
  CharString versionString = newCharString();
  snprintf(versionString, STRING_LENGTH, "This is %s, version %d.%d.%d.", PROGRAM_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  return versionString;
}

int main(int argc, char** argv) {
  initEventLogger();

  // Input/Output sources, plugin chain, and other required objects
  InputSource inputSource;

  ProgramOptions programOptions = newProgramOptions();
  if(!parseCommandLine(programOptions, argc, argv)) {
    logCritical("Error parsing command line");
    return -1;
  }

  for(int i = 0; i < NUM_OPTIONS; i++) {
    ProgramOption option = programOptions[i];
    if(option->enabled) {
      if(option->index == OPTION_HELP) {
        printf("Usage: %s (options), where options include:\n", getFileBasename(argv[0]));
        printProgramOptions(programOptions);
        return RETURN_CODE_NOT_RUN;
      }
      else if(option->index == OPTION_VERSION) {
        CharString versionString = getNewVersionString();
        printf("%s\nCopyright (c) %d, %s. All rights reserved.\n\n", versionString, buildYear(), COPYRIGHT_HOLDER);
        free(versionString);

        CharString wrappedLicenseInfo = newCharStringLong();
        wrapCharStringForTerminal(LICENSE_STRING, wrappedLicenseInfo, 0);
        printf("%s\n\n", wrappedLicenseInfo);
        free(wrappedLicenseInfo);

        return RETURN_CODE_NOT_RUN;
      }
      else if(option->index == OPTION_VERBOSE) {
        setLogLevel(LOG_DEBUG);
      }
      else if(option->index == OPTION_QUIET) {
        setLogLevel(LOG_CRITICAL);
      }
      else if(option->index == OPTION_COLOR_LOGGING) {
        if(isStringEmpty(option->argument)) {
          setLoggingColor(COLOR_TYPE_DARK);
        }
        else if(!strncmp(option->argument, "dark", STRING_LENGTH)) {
          setLoggingColor(COLOR_TYPE_DARK);
        }
        else if(!strncmp(option->argument, "light", STRING_LENGTH)) {
          setLoggingColor(COLOR_TYPE_LIGHT);
        }
        else {
          logCritical("Unknown color scheme");
          setLoggingColor(COLOR_TYPE_PLAIN);
        }
      }
      else if(option->index == OPTION_INPUT_SOURCE) {
        InputSourceType inputSourceType = guessInputSourceType(option->argument);
        inputSource = newInputSource(inputSourceType);
      }
    }
  }
  freeProgramOptions(programOptions);

  CharString hello = getNewVersionString();
  logInfo(hello);
  free(hello);

  return RETURN_CODE_SUCCESS;
}