include(FetchContent)
FetchContent_Declare(
  aarand
  GIT_REPOSITORY https://github.com/LTLA/aarand
  GIT_TAG master #^1.0.0
)

FetchContent_MakeAvailable(aarand)
