#include "Lighting.h"
#include "main.h"
#include "EXPANDER_MCP23017.h"
#include <stdint.h>

extern volatile char Expander_flag;

/*
 * Button_Light_Handler
 * 
 * Fonction de gestion des boutons sur le port B de l'expander
 * Lit le registre INTCAPB pour détecter les boutons appuyés et contrôle les lampes
 * 
 * Comportement:
 *  - BTN4 appuyé: allume toutes les lampes (LAMP1, LAMP2, LAMP3, LAMP4)
 *  - BTN3 appuyé: éteint toutes les lampes (LAMP1, LAMP2, LAMP3, LAMP4)
 *  - BTN2 appuyé: éteint LAMP1
 *  - BTN1 appuyé: allume LAMP1
 */
void Button_Light_Handler(void)
{
    uint8_t current_button;

    // Vérifier si le drapeau d'interrupt de l'expander est activé
    if (!Expander_flag)
        return;

    // Lire le registre de capture d'interrupt pour obtenir l'état des boutons
    current_button = Expander_Read(INTCAPB);

    // Vérifier quel bouton a été appuyé (logique inversée: 0 = appuyé)
    if (!Is_BIT_SET(current_button, BTN4_PIN))
    {
        // BTN4 appuyé: allumer toutes les lampes
        LAMP1_ON
        LAMP2_ON
        LAMP3_ON
        LAMP4_ON
    }
    else if (!Is_BIT_SET(current_button, BTN3_PIN))
    {
        // BTN3 appuyé: éteindre toutes les lampes
        LAMP1_OFF
        LAMP2_OFF
        LAMP3_OFF
        LAMP4_OFF
    }
    else if (!Is_BIT_SET(current_button, BTN2_PIN))
    {
        // BTN2 appuyé: éteindre LAMP1
        LAMP1_OFF
    }
    else if (!Is_BIT_SET(current_button, BTN1_PIN))
    {
        // BTN1 appuyé: allumer LAMP1
        LAMP1_ON
    }

    // Lire les registres pour clear l'interrupt (clear la condition d'interrupt)
    Expander_Read(INTCAPB);
    Expander_Read(GPIOB);

    // Réinitialiser le drapeau
    Expander_flag = FALSE;
}

