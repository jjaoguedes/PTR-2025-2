#ifndef SIM_ROBOT_H
#define SIM_ROBOT_H

#ifdef __cplusplus
extern "C" {
#endif

// Parâmetros do simulador
typedef struct {
    double dt;     // Passo (s) — aqui: 0.05
    double t_end;  // Tempo final (s) — aqui: 20.0
    double D;      // Diâmetro do robô (m) — aqui: 0.30
} SimParams;

/**
 * Inicializa o módulo com parâmetros (dt, t_end, D).
 * Deve ser chamado antes de iniciar a simulação.
 */
void simrobot_init(const SimParams *params);

/**
 * Encerra o módulo (destroy de mutex/conds). Opcional.
 */
void simrobot_destroy(void);

/**
 * Inicia a thread de simulação (mantém estado interno x).
 * Retorna 0 em sucesso.
 */
int  simrobot_start(void);

/**
 * Aguarda a thread de simulação terminar.
 */
void simrobot_join(void);

/**
 * Publica a entrada u_k = [v, w] válida no intervalo [t_k, t_k+dt), com rótulo de sequência 'seq'.
 * Esta é a ÚNICA forma de comunicação de I/O -> Simulação.
 */
void simrobot_publish_input(double t_k, double v, double w, int seq);

/**
 * Aguarda a saída y_{k+1} rotulada com 'expected_seq' e a retorna.
 * Saída: t_{k+1}, yx, yy, theta (theta é opcional/para depuração).
 * Esta é a ÚNICA forma de comunicação de Simulação -> I/O.
 */
void simrobot_wait_output(int expected_seq, double *t_next, double *yx, double *yy, double *theta);

/**
 * (Helper opcional) Gera u(t) segundo a lei por trechos do laboratório:
 *  t < 0: v=0, w=0
 *  0 <= t < 10: v=1, w=+0.2*pi
 *  t >= 10: v=1, w=-0.2*pi
 */
void simrobot_generate_u(double t, double *v, double *w);

#ifdef __cplusplus
}
#endif

#endif // SIM_ROBOT_H
