#include <mictcp.h>
#include <api/mictcp_core.h>

#define MAX_SOCKET 5
#define TIMEOUT 10000 

mic_tcp_sock sockets[MAX_SOCKET] ; // table de sockets

/*
 * Permet de créer un socket entre l’application et MIC-TCP
 * Retourne le descripteur du socket ou bien -1 en cas d'erreur
 */

int nb_fd = 0;

int mic_tcp_socket(start_mode sm)
{
    struct mic_tcp_sock sock;
    
    int result = -1;
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    result = initialize_components(sm); /* Appel obligatoire */
    set_loss_rate(0);
    
    sock.fd = nb_fd; // definir le numero de soc
    sock.state = IDLE; 
    nb_fd += 1;
    sockets[sock.fd] = sock; // mettre le sock dans la table de sockets.
    result = sock.fd;  
    return result;
}

/*
 * Permet d’attribuer une adresse à un socket.
 * Retourne 0 si succès, et -1 en cas d’échec
 */
int mic_tcp_bind(int socket, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");

    for (int i = 0; i < MAX_SOCKET; i++){
        if (i != socket && sockets[socket].local_addr.port == addr.port){ 
            return -1;
        } 
    } 
    sockets[socket].local_addr = addr;
    return 0; 

}

/*
 * Met le socket en état d'acceptation de connexions
 * Retourne 0 si succès, -1 si erreur
 */
int mic_tcp_accept(int socket, mic_tcp_sock_addr* addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    return 0;
}

/*
 * Permet de réclamer l’établissement d’une connexion
 * Retourne 0 si la connexion est établie, et -1 en cas d’échec
 */
int mic_tcp_connect(int socket, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    if (socket > MAX_SOCKET){
        return -1;
    }  
    sockets[socket].remote_addr = addr; 
    return 0;
}

/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */
int mic_tcp_send (int mic_sock, char* mesg, int mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    
    // pdu message
    mic_tcp_pdu pdu;
    pdu.payload.size = mesg_size;
    pdu.payload.data = mesg;
    pdu.header.dest_port = sockets[mic_sock].remote_addr.port;
    
    // envoyer le pdu a address remote ip
    int send = IP_send(pdu, sockets[mic_sock].remote_addr.ip_addr );
    return send;

}

/*
 * Permet à l’application réceptrice de réclamer la récupération d’une donnée
 * stockée dans les buffers de réception du socket
 * Retourne le nombre d’octets lu ou bien -1 en cas d’erreur
 * NB : cette fonction fait appel à la fonction app_buffer_get()
 */
int mic_tcp_recv (int socket, char* mesg, int max_mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");

    // message reçu
    mic_tcp_payload payload;
    payload.size = max_mesg_size;
    payload.data = mesg;

    // recevoir le message
    int recv = app_buffer_get(payload);
    return recv;
}

/*
 * Permet de réclamer la destruction d’un socket.
 * Engendre la fermeture de la connexion suivant le modèle de TCP.
 * Retourne 0 si tout se passe bien et -1 en cas d'erreur
 */
int mic_tcp_close (int socket)
{
    printf("[MIC-TCP] Appel de la fonction :  "); printf(__FUNCTION__); printf("\n");
    sockets[socket].state = CLOSED; 
    return 0;
}

/*
 * Traitement d’un PDU MIC-TCP reçu (mise à jour des numéros de séquence
 * et d'acquittement, etc.) puis insère les données utiles du PDU dans
 * le buffer de réception du socket. Cette fonction utilise la fonction
 * app_buffer_put().
 */
void process_received_PDU(mic_tcp_pdu pdu, mic_tcp_ip_addr local_addr, mic_tcp_ip_addr remote_addr)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");

    app_buffer_put(pdu.payload);
}
