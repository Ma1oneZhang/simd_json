#
# Created by ziyang on 12/17/23.
#
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        f8d7d77 # release-1.10.0
        OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(googletest)