/*
 *      fm.c
 *
 *      Copyright 2009 - 2012 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/**
 * SECTION:fm
 * @short_description: Libfm initialization
 * @title: Libfm
 *
 * @include: libfm/fm.h
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glib/gi18n-lib.h>
#include "fm.h"

#ifdef HAVE_ACTIONS
#include "fm-actions.h"
#endif

#ifdef USE_UDISKS
#include "udisks/fm-udisks.h"
#endif

#include "vfs/fm-vfs-menu.h"
#include "vfs/fm-vfs-search.h"

GQuark fm_qdata_id = 0;

/**
 * fm_init
 * @config: configuration file data
 *
 * Initializes libfm data.
 *
 * Returns: %TRUE.
 *
 * Since: 0.1.0
 */
gboolean fm_init(FmConfig* config)
{
#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
#endif

#if !GLIB_CHECK_VERSION(2, 32, 0)
    g_thread_init(NULL);
#endif
    g_thread_pool_set_max_idle_time(10000); /* is 10 sec enough? */

    if(config)
        fm_config = (FmConfig*)g_object_ref(config);
    else
    {
        /* create default config object */
        fm_config = fm_config_new();
        fm_config_load_from_file(fm_config, NULL);
    }

    _fm_file_init();
    fm_file_add_vfs("menu", &_fm_vfs_menu_init_table);
    fm_file_add_vfs("search", &_fm_vfs_search_init_table);
    _fm_path_init();
    _fm_icon_init();
    _fm_monitor_init();
    _fm_mime_type_init();
    _fm_file_info_init(); /* should be called only after _fm_mime_type_init() */
    _fm_folder_init();
    _fm_archiver_init();
    _fm_thumbnailer_init(); // must be called after mime-types are initialized
    _fm_thumbnail_loader_init();
    _fm_terminal_init(); /* should be called after config initialization */
    _fm_templates_init();

#ifdef HAVE_ACTIONS
	/* generated by vala */
    _fm_file_actions_init();
#endif

#ifdef USE_UDISKS
    _fm_udisks_init();
#endif

    fm_qdata_id = g_quark_from_static_string("fm_qdata_id");

    return TRUE;
}

/**
 * fm_finalize
 *
 * Frees libfm data.
 *
 * Since: 0.1.0
 */
void fm_finalize(void)
{
#ifdef HAVE_ACTIONS
	/* generated by vala */
    _fm_file_actions_finalize();
#endif

    _fm_templates_finalize();
    _fm_terminal_finalize();
    _fm_thumbnail_loader_finalize();
    _fm_thumbnailer_finalize(); /* need to be before fm_mime_type_finalize() */
    _fm_archiver_finalize();
    _fm_folder_finalize();
    _fm_file_info_finalize();
    _fm_mime_type_finalize();
    _fm_monitor_finalize();
    _fm_icon_finalize();
    _fm_path_finalize();
    _fm_file_finalize();

#ifdef USE_UDISKS
    _fm_udisks_finalize();
#endif

    fm_config_save(fm_config, NULL);
    g_object_unref(fm_config);
    fm_config = NULL;
}
