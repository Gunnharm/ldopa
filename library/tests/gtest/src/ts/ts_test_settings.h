////////////////////////////////////////////////////////////////////////////////
/// /file
/// \brief     Settings file for testing TS.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      31.07.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// HOW TO SET A TEST ENVIRONMENT:
/// one need to define PN_TEST_MODELS_BASE_DIR directive to set it to a
/// machine/user specific value
///
////////////////////////////////////////////////////////////////////////////////


#ifndef XI_LDOPA_TS_TS_TEST_SETTINGS_H_
#define XI_LDOPA_TS_TS_TEST_SETTINGS_H_


// if no TS_TEST_MODELS_BASE_DIR is set, use default value
#ifndef TS_TEST_MODELS_BASE_DIR

#define TS_TEST_MODELS_BASE_DIR "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/tests/gtest/work_files/"

#endif


#endif // XI_LDOPA_TS_TS_TEST_SETTINGS_H_
