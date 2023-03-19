option(POWERIT_FETCH_EXTERN "" ON)

if(POWERIT_FETCH_EXTERN)
    include(FetchContent)
    FetchContent_Declare(
      aarand
      GIT_REPOSITORY https://github.com/LTLA/aarand
      GIT_TAG 84d48b65d49ce8b844398f11aff3015b86e17197
    )

    FetchContent_MakeAvailable(aarand)
else()
    find_package(ltla_aarand CONFIG REQUIRED)
endif()
