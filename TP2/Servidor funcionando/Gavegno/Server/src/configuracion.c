#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include "../comons/config.h"
#include "../inc/ServidorWeb.h"
#include "../inc/configuracion.h"

void configuracion_init(config_t *serverConf)
{
    t_config *config;

    //Leo el archivo de configuracion
    config = config_create(CONFIG_RUTA);
    if (config != NULL)
    {
        //El archivo existe, lo leo.
        if (config_has_property(config, "CONEXIONES_MAX") == true)
        {
            serverConf->conexiones_max = config_get_int_value(config, "CONEXIONES_MAX");
        }
        else
        {
            serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        }
        if (config_has_property(config, "BACKLOG") == true)
        {
            serverConf->backlog = config_get_int_value(config, "BACKLOG");
        }
        else
        {
            serverConf->backlog = BACKLOG_DEFAULT;
        }
        if (config_has_property(config, "MUESTREO_SENSOR") == true)
        {
            serverConf->muestreo = config_get_int_value(config, "MUESTREO_SENSOR");
        }
        else
        {
            serverConf->muestreo = MUESTREO_SENSOR;
        }
    }
    else
    {
        //No existe el archivo de configuracion, utilizo valores por default
        printf("No existe el archivo de configuracion\n");
        serverConf->backlog = BACKLOG_DEFAULT;
        serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        serverConf->muestreo = MUESTREO_SENSOR;
    }

    serverConf->conexiones = 0;
    
    printf("|*******************************************************\n");
    printf("|                                                       \n");
    printf("|Cantidad maxima de conexiones      = %d                \n", serverConf->conexiones_max);
    printf("|Backlog                            = %d                \n", serverConf->backlog);
    printf("|Muestreo del filtro                = %d                \n", serverConf->muestreo);
    printf("|Cantidad de conexiones actuales    = %d                \n", serverConf->conexiones);
    printf("|                                                       \n");
    printf("|*******************************************************\n");
}

void configuracion_reload( config_t *serverConf)
{
    t_config *config;

    //Leo el archivo de configuracion
    config = config_create(CONFIG_RUTA);
    if (config != NULL)
    {
        //El archivo existe, lo leo.
        if (config_has_property(config, "CONEXIONES_MAX") == true)
        {
            serverConf->conexiones_max = config_get_int_value(config, "CONEXIONES_MAX");
        }
        else
        {
            serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        }
        if (config_has_property(config, "MUESTREO_SENSOR") == true)
        {
            serverConf->muestreo = config_get_int_value(config, "MUESTREO_SENSOR");
        }
        else
        {
            serverConf->muestreo = MUESTREO_SENSOR;
        }
    }
    else
    {
        //No existe el archivo de configuracion, utilizo valores por default
        printf("No existe el archivo de configuracion\n");
        serverConf->backlog = BACKLOG_DEFAULT;
        serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        serverConf->muestreo = MUESTREO_SENSOR;
    }

    printf("|*******************************************************\n");
    printf("|                                                       \n");
    printf("|Cantidad maxima de conexiones =    %d                  \n", serverConf->conexiones_max);
    printf("|Backlog =                          %d                  \n", serverConf->backlog);
    printf("|Muestreo del filtro =              %d                  \n", serverConf->muestreo);
    printf("|                                                       \n");
    printf("|*******************************************************\n");
}