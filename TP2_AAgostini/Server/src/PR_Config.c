#include "../inc/PR_Config.h"


extern config_t *serverConf;
extern int flag_exit;
extern int flag_config;


/*Carga de configuracion inicial*/
void config_init(config_t *serverConf)
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
}

/*Handler de configuracion*/
void config_handler(void)
{
    printf("Manejador de la configuracion\n");

    while (!flag_exit)
    {
        if (flag_config == true)
        {
            //--------Configuracion del Server----------
            semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
            config_reload(configuracionServer);
            semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
            flag_config = false;
        }
        sleep(1);
    }
}

/* Recargar configuracion */
void config_reload( config_t *serverConf)
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

    printf("*****************************\n");
    printf("Nueva Configuracion cargada  \n");
    printf("    Max Con. =    %d        \n", serverConf->conexiones_max);
    printf("    Backlog  =    %d        \n", serverConf->backlog);
    printf("    Muestras =    %d        \n", serverConf->muestreo);
    printf("*****************************\n");
}