/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LIBIMPORTWIZARDPRIVATE_EXPORT_H
#define LIBIMPORTWIZARDPRIVATE_EXPORT_H

#include "libimportwizard_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
# ifndef LIBIMPORTWIZARD_TESTS_EXPORT
#  define LIBIMPORTWIZARD_TESTS_EXPORT LIBIMPORTWIZARD_EXPORT
# endif
#else /* not compiling tests */
# define LIBIMPORTWIZARD_TESTS_EXPORT
#endif

#endif
