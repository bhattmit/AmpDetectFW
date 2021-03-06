#include <cstdint>
#include "Pid.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Pid::Service (uint32_t nPeriod_ms, int32_t nSetpoint_mC, int32_t nProcessVar, int32_t nProcessErr, int32_t* pControlVar)
{
    int32_t nNewAccInt;         //Proposed new integrator value
    
    //Once the temperature is stable, it stays stable.
    if (_bIsStable == false)
    {
        //Determine if the temperature is stable.
        if ((nProcessVar < (nSetpoint_mC + _nStableBand_mC)) &&
            (nProcessVar > (nSetpoint_mC - _nStableBand_mC)))
        {
            _nStableTimer_ms += nPeriod_ms;
            if (_nStableTimer_ms >= _nStablePeriod_ms)
                _bIsStable = true;
        }
    }

    //Compute new integrator and the final control output.
    nNewAccInt = _nIntegrationAcc + nProcessErr;
    *pControlVar = (((_nKp * nProcessErr) / 1000) + ((_nKi * nNewAccInt) / 1000)) >> _nShift;

    //Check for saturation.  In the event of saturation in any one direction,
    //inhibit saving the integrator if doing so would deepen the saturation.
    bool bIntegratorOK = true;

    //Positive saturation?
    if (*pControlVar > _nMaxPwr_uA)
    {
        //Clamp the output
        *pControlVar =_nMaxPwr_uA;

        //Error is the same sign? Inhibit integration.
        if (nProcessErr > 0)
            bIntegratorOK = false;
    }
    else if (*pControlVar < _nMinPwr_uA)    //Negative saturation.
    {
        *pControlVar = _nMinPwr_uA;

        if (nProcessErr < 0)
            bIntegratorOK = false;
    }

    //Update the integrator if allowed.
    if (bIntegratorOK)
        _nIntegrationAcc = nNewAccInt;

    return _bIsStable;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pid::ResetStableFlg ()
{
    _bIsStable = false;
    _nStableTimer_ms = 0;
}

