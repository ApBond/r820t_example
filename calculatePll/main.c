#include <stdio.h>
#include <stdint.h>

#define XTAL_FREQ 28800000

void R820T2_set_pll(uint32_t freq) {
    const uint32_t vco_min = 1770000000;
    const uint32_t vco_max = 3900000000;
    uint32_t pll_ref = (XTAL_FREQ >> 1);
    uint32_t pll_ref_2x = XTAL_FREQ;

    uint32_t vco_exact;
    uint32_t vco_frac;
    uint32_t con_frac;
    uint32_t div_num;
    uint32_t n_sdm;
    uint16_t sdm;
    uint8_t ni;
    uint8_t si;
    uint8_t nint;

    /* Calculate VCO output divider */
    for(div_num = 0; div_num < 5; div_num++) {
        vco_exact = freq << (div_num + 1);
        if(vco_exact >= vco_min && vco_exact <= vco_max) {
            break;
        }
    }

    /* Calculate the integer PLL feedback divider */
    vco_exact = freq << (div_num + 1);
    nint = (uint8_t) ((vco_exact + (pll_ref >> 16)) / pll_ref_2x);
    vco_frac = vco_exact - pll_ref_2x * nint;

    nint -= 13;
    ni = (nint >> 2);
    si = nint - (ni << 2);

    printf("Vco outpud divider (set to 0x10): %d<<5\n",div_num);
    /* Set the vco output divider */
    //R820T2_write_cache_mask(0x10, (uint8_t) (div_num << 5), 0xE0);

    printf("PLL feedback integer divider (set to 0x14):(%d + (%d << 6))\n",ni,si);
    /* Set the PLL Feedback integer divider */
    //R820T2_write_reg(0x14, (uint8_t) (ni + (si << 6)));

    /* Update Fractional PLL */
    if(vco_frac == 0) {
        /* Disable frac pll */
        
        //R820T2_write_cache_mask(0x12, 0x08, 0x08);
    } else {
        /* Compute the Sigma-Delta Modulator */
        vco_frac += pll_ref >> 16;
        sdm = 0;
        for(n_sdm = 0; n_sdm < 16; n_sdm++) {
            con_frac = pll_ref >> n_sdm;
            if (vco_frac >= con_frac) {
                sdm |= (uint16_t) (0x8000 >> n_sdm);
                vco_frac -= con_frac;
                if (vco_frac == 0)
                    break;
            }
        }
       
        /* Update Sigma-Delta Modulator */
        printf("Update sigma-delta modulator\n");
        printf("write set to 0x15 = %d & 0xFF\n",sdm);
        printf("write set to 0x16 = %d >> 8\n",sdm);
        //R820T2_write_reg(0x15, (uint8_t)(sdm & 0xFF));
        //R820T2_write_reg(0x16, (uint8_t)(sdm >> 8));

        /* Enable frac pll */
        //R820T2_write_cache_mask(0x12, 0x00, 0x08);
    }
}

int main()
{
    uint32_t inpFreq;
    printf("Write reference frequency\n");
    scanf("%d",&inpFreq);
    printf("RT pll diveder calculate\n");
    R820T2_set_pll(inpFreq);
    return 0;
}
