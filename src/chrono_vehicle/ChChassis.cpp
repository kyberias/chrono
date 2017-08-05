// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban
// =============================================================================
//
// Base class for the chassis vehicle subsystem.
//
// =============================================================================

#include "chrono/assets/ChSphereShape.h"

#include "chrono_vehicle/ChChassis.h"

namespace chrono {
namespace vehicle {

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ChChassis::ChChassis(const std::string& name, bool fixed) : ChPart(name), m_fixed(fixed) {}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ChVector<> ChChassis::GetPointLocation(const ChVector<>& locpos) const {
    return m_body->GetFrame_REF_to_abs().TransformPointLocalToParent(locpos);
}

ChVector<> ChChassis::GetPointVelocity(const ChVector<>& locpos) const {
   return m_body->GetFrame_REF_to_abs().PointSpeedLocalToParent(locpos);
}

ChVector<> ChChassis::GetPointAcceleration(const ChVector<>& locpos) const {
    ChVector<> acc_abs = m_body->GetFrame_REF_to_abs().PointAccelerationLocalToParent(locpos);
    return m_body->GetFrame_REF_to_abs().TransformDirectionParentToLocal(acc_abs);
}

// -----------------------------------------------------------------------------
// Return the global driver position
// -----------------------------------------------------------------------------
ChVector<> ChChassis::GetDriverPos() const {
    return m_body->GetFrame_REF_to_abs().TransformPointLocalToParent(GetLocalDriverCoordsys().pos);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ChChassis::Initialize(ChSystem* system,
                           const ChCoordsys<>& chassisPos,
                           double chassisFwdVel,
                           int collision_family) {
    m_body = std::shared_ptr<ChBodyAuxRef>(system->NewBodyAuxRef());
    m_body->SetIdentifier(0);
    m_body->SetNameString(m_name + " body");
    m_body->SetMass(GetMass());
    m_body->SetFrame_COG_to_REF(ChFrame<>(GetLocalPosCOM(), ChQuaternion<>(1, 0, 0, 0)));
    m_body->SetInertia(GetInertia());
    m_body->SetBodyFixed(m_fixed);

    m_body->SetFrame_REF_to_abs(ChFrame<>(chassisPos));
    m_body->SetPos_dt(chassisFwdVel * chassisPos.TransformDirectionLocalToParent(ChVector<>(1, 0, 0)));

    system->Add(m_body);

    // Add pre-defined markers (driver position and COM) on the chassis body.
    AddMarker("driver position", GetLocalDriverCoordsys());
    AddMarker("COM", ChCoordsys<>(GetLocalPosCOM()));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ChChassis::AddMarker(const std::string& name, const ChCoordsys<>& pos) {
    // Do nothing if the chassis is not yet initialized
    if (!m_body)
        return;

    // Note: marker local positions are assumed to be relative to the centroidal frame
    //       of the associated body.
    auto pos_com = m_body->GetFrame_REF_to_COG().GetCoord().TransformLocalToParent(pos);

    // Create the marker, attach it to the chassis body, add it to the list
    auto marker = std::make_shared<ChMarker>();
    marker->SetNameString(m_name + " " + name);
    marker->Impose_Rel_Coord(pos_com);
    m_body->AddMarker(marker);
    m_markers.push_back(marker);
}

}  // end namespace vehicle
}  // end namespace chrono
