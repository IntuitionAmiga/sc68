/** Setup function for ym blep synthesis engine.
 *
 *    The ym_blep_setup() function sets ym blep synthesis engine for
 *    this ym emulator instance.
 *
 *  @parm    ym  ym emulator instance to setup
 *  @retval   0  on success
 *  @retval  -1  on failure
 */
int ym_blep_setup(ym_t * const ym);

enum {
    MAX_BLEPS = 512
};

typedef struct {
    s32 count;
    u32 event;
    u16 flip_flop;
    u16 tonemix;
    u16 noisemix;
    u16 envmask;
    u16 volmask;
} ym_blep_tone_t;

typedef struct {
    u32 stamp;
    s32 level;
} ym_blep_blep_state_t;

typedef struct {
    /* sampling parameters */
    u32 cycles_per_sample; /* 8 bit fixed point ym clocks */
    u32 cycles_to_next_sample;

    /* hp filter */
    s32 hp;

    /* blep stuff */
    s32 global_output_level;
    u32 blep_idx;
    u32 systemtime;
    ym_blep_blep_state_t blepstate[MAX_BLEPS];

    /* subsystem states */
    ym_blep_tone_t tonegen[3];
    u32 noise_event;
    s32 noise_count;
    u32 noise_state;
    u16 noise_output;

    u32 env_event;
    s32 env_count;
    u8 env_state;
    u16 env_output;

} ym_blep_t;
