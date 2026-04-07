#include <stdlib.h>
#include <stdio.h>
#include <gpib/ib.h>
#include "gpib.h"
#include "core.h"

#define GPIB_BUFFER_SIZE 256

/* ------------------------------------------------------------------ */
/* 
TODO: Il faut que le programme utilise la strucutre local pour stocker les donnees lues du corps noir, 
      idealement le thread devrait utiliser le mutex a un seul endroit pour realiser la transition de donnee local -> global. 
*/

GpibLocalData local_controller = {
    .target_temp   = 0.0,
    .emitter_temp  = 0.0,
    .target_index  = 0,
    .state         = MASTER_OFFLINE_DEVICE_OFFLINE,
    .ud            = -1
};

/*
Premiere fonction a appeler avec d'utiliser les API de controle du SR80,
on fournis en argument l'addresse du master et du device (j'ai choisis de fixer MASTER = 0 et DEVICE = 1),
La fonction tente d'ouvrir le device, la fonction ibdev ne verifie que si un descripteur usb-gpib est present compatible a la bibliotheque,
j'ai donc du ajouter une verification supplementaire en envoyant une commande de lecture de la version du SR80.
*/
DeviceState gpib_init(int master_addr, int dev_addr)
{
    const int sad      = 0;
    const int send_eoi = 1;
    const int eos_mode = 0;
    const int timeout  = T1s;
    char response[GPIB_BUFFER_SIZE];

    printf("Trying to open pad=%i on /dev/gpib%i...\n", dev_addr, master_addr);

    local_controller.ud = ibdev(master_addr, dev_addr, sad, timeout, send_eoi, eos_mode);
    if (local_controller.ud < 0)
    {
        fprintf(stderr, "ibdev failed\n");
        local_controller.state = MASTER_OFFLINE_DEVICE_OFFLINE;
        local_controller.ud = -1;
        return MASTER_OFFLINE_DEVICE_OFFLINE;
    }

    local_controller.state = MASTER_ONLINE_DEVICE_OFFLINE;

    if (gpib_write_read("RV", response) < 0)
    {
        fprintf(stderr, "Device not responding (RV command failed)\n");
        ibonl(local_controller.ud, 0);
        local_controller.state = MASTER_ONLINE_DEVICE_OFFLINE;
        local_controller.ud = -1;
        return MASTER_ONLINE_DEVICE_OFFLINE;
    }

    /* Vérification que la réponse contient "SR80" */
    if (strstr(response, "SR80") == NULL)
    {
        fprintf(stderr, "Unexpected device response: %s\n expected response was SR80", response);
        ibonl(local_controller.ud, 0);
        local_controller.state = MASTER_ONLINE_DEVICE_OFFLINE;
        local_controller.ud = -1;
        return MASTER_ONLINE_DEVICE_OFFLINE;
    }

    printf("Corps noir SR80 confirmé\nréponse: %s\n", response);
    local_controller.state = MASTER_ONLINE_DEVICE_ONLINE;
    return MASTER_ONLINE_DEVICE_ONLINE;
}
/*======================================================================================================*/
/*
Les commandes doivent etre envoyees en ASCII, on utilisera donc les fonctions ibwrt et ibrd pour communiquer avec le corps noir.
Les API fournie par Sring.h sont la pour nous aider a construire les commandes et parser les reponses, mais la logique de parsing doit etre implementee manuellement.
l'API ibwrt et ibrd sont des fonctions bas niveau qui ne font que transmettre les octets, elles ne gèrent pas les terminaisons de ligne ou les conversions de format,
c'est à nous de nous assurer que les commandes sont correctement formatées et que les réponses sont correctement interprétées.
*/
int gpib_write(const char *command)
{

	printf("sending string: %s\n", command);
    ibwrt(local_controller.ud, command, strlen(command));
	if((ThreadIbsta() & ERR))
	{
        fprintf(stderr, "ibwrt error\n");
		return -1;
	}
	return 0;
}
/*======================================================================================================*/
/*
Fonction de bas niveau qui permet de lire la reponse brute du corps noir, elle ne fait que remplir le buffer de reponse et verifier les erreurs de lecture.
*/
int gpib_read(char *response)
{
    memset(response, 0, GPIB_BUFFER_SIZE);
    ibrd(local_controller.ud, response, GPIB_BUFFER_SIZE - 1);
    if (ThreadIbsta() & ERR)
    {
        fprintf(stderr, "ibrd error\n");
        return -1;
    } 
    return 0;   
}
/*======================================================================================================*/
/*
Pour les acitons de lecture il est necessaire d'envoyer une commande pour indiquer au SR80 ce que l'on souhaite lire.
cette fonction est la pour alleger le code et la logique du polling. 
*/
int gpib_write_read(const char *command, char *response)
{
    if (gpib_write(command) < 0) return -1;
    return gpib_read(response);
}
/*======================================================================================================*/

/* 
Fonction utiliser dans le thread de polling, elle utilise gpib_write_read et stock les resultats dans une structure de transition,
pour ne pas monopoliser le mutex global pendant les lectures gpib, ce qui permet a l'UI de rester réactive et d'éviter les risques de deadlock. 
*/
int gpib_read_all()
{
    char response[GPIB_BUFFER_SIZE];

    if (gpib_write_read("RT", response) < 0) {
        fprintf(stderr, "Failed to read RT from GPIB device\n");
        return -1;
    }
    local_controller.target_temp = atof(response);
    printf("Target temperature: %f\n", local_controller.target_temp);

    if (gpib_write_read("RE", response) < 0) {
        fprintf(stderr, "Failed to read RE from GPIB device\n");
        return -1;
    } 
    local_controller.emitter_temp = atof(response);
    printf("Emitter temperature: %f\n", local_controller.emitter_temp);

    if (gpib_write_read("RA", response) < 0) {
        fprintf(stderr, "Failed to read RA from GPIB device\n");
        return -1;
    } 
    local_controller.target_index = atoi(response);
    printf("Target position: %d/12\n", local_controller.target_index);
    

    // Ajouter lecture utile ici
    
    return 0;
}
/*======================================================================================================*/
/*
N'est pas ops pour l'instant, doit etre appelee lors de la fermeture du programme.
elle modifie les variables locales... est ce correcte ?
*/
void cleanup_and_quit(void)
{
    // Libère le device GPIB si connecté
    if (local_controller.state == MASTER_ONLINE_DEVICE_ONLINE)
    {
        ibonl(local_controller.ud, 0);
        local_controller.state = MASTER_OFFLINE_DEVICE_OFFLINE;
    }

}

