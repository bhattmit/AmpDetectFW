#ifndef __Site_H
#define __Site_H

#include <cstdint>
#include <vector>
#include "PcrProtocol.h"
#include "Pid.h"
#include "SysStatus.h"
#include "ThermalDriver.h"
#include "OpticsDriver.h"
#include "HostMessages.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Site
{
public:
    enum    {kPidTick_ms = 100};
    enum    {kThermalAcqPeriod_ms = 250};
    enum    {kMaxThermalRecs = 1};
    
    enum    ManCtrlState : uint32_t
            {kIdle = 0, kSetpointControl, kOpticsLedControl};
    
    Site(uint32_t nSiteIdx = 0);
    
    void                Execute();
    void                ManualControl();
    bool                GetRunningFlg() const {return _siteStatus.GetRunningFlg();}
    uint32_t            GetNumThermalRecs() const;
    ThermalRec          GetAndDelNextThermalRec();
    uint32_t            GetNumOpticsRecs() const;
    const OpticsRec*    GetOpticsRec(uint32_t nIdx) const;
    void                SetPcrProtocol(const PcrProtocol& p)   {_pcrProtocol = p;}
    const SiteStatus&   GetStatus();
    OpticsDriver&       GetOpticsDrvPtr()   {return _opticsDrv;}
    
    ErrCode             StartRun(bool bMeerstetterPid);
    ErrCode             StopRun();
    ErrCode             SetManControlSetpoint(int32_t nSp_mC);
    ErrCode             SetPidParams(uint32_t nKp, uint32_t nKi, uint32_t nKd);
    ErrCode             SetOpticsLed(uint32_t nChanIdx, uint32_t nIntensity, uint32_t nDuration);
    
protected:
  
private:
    uint32_t                    _nSiteIdx;
    ThermalDriver               _thermalDrv;
    OpticsDriver                _opticsDrv;
    PcrProtocol                 _pcrProtocol;
    bool                        _bMeerstetterPid;
    Pid                         _pid;
    SiteStatus                  _siteStatus;
    std::vector<OpticsRec>      _arOpticsRecs;
    std::vector<ThermalRec>     _arThermalRecs;
    uint32_t                    _nThermalAcqTimer_ms;
    uint32_t                    _nThermalRecPutIdx;
    uint32_t                    _nThermalRecGetIdx;
    
    ManCtrlState                _nManControlState;
    uint32_t                    _nDuration_us;
    int32_t                     _nManControlSetpoint_mC;
};

#endif // __Site_H
