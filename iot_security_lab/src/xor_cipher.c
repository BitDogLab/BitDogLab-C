// Inclusão do arquivo de cabeçalho que contém a declaração da função
#include "include/xor_cipher.h"

/**
 * Função para aplicar cifra XOR (criptografia/decifração)
 * 
 * @param input  Ponteiro para os dados de entrada (texto claro ou cifrado)
 * @param output Ponteiro para armazenar o resultado (deve ter tamanho >= len)
 * @param len    Tamanho dos dados em bytes
 * @param key    Chave de 1 byte (0-255) para operação XOR
 * 
 * Funcionamento:
 * - Aplica operação XOR bit-a-bit entre cada byte do input e a chave
 * - XOR é reversível: mesma função para cifrar e decifrar
 * - Criptografia fraca (apenas para fins didáticos ou ofuscação básica)
 */
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key) {
    // Loop por todos os bytes dos dados de entrada
    for (size_t i = 0; i < len; ++i) {
        // Operação XOR entre o byte atual e a chave
        // Armazena resultado no buffer de saída
        output[i] = input[i] ^ key;
    }
}