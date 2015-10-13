//
//  Created by Bradley Austin Davis 2015/10/09
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_Controllers_Impl_RouteBuilderProxy_h
#define hifi_Controllers_Impl_RouteBuilderProxy_h

#include <QtCore/QObject>
#include "../Filter.h"
#include "../Route.h"
#include "../Mapping.h"

class QJSValue;
class QScriptValue;

namespace controller {

class NewControllerScriptingInterface;

class RouteBuilderProxy : public QObject {
        Q_OBJECT
    public:
        RouteBuilderProxy(NewControllerScriptingInterface& parent, Mapping::Pointer mapping, Route::Pointer route)
            : _parent(parent), _mapping(mapping), _route(route) { }

        Q_INVOKABLE void to(const QJSValue& destination);
        Q_INVOKABLE void to(const QScriptValue& destination);

        Q_INVOKABLE QObject* filter(const QJSValue& expression);
        Q_INVOKABLE QObject* filter(const QScriptValue& expression);
        Q_INVOKABLE QObject* clamp(float min, float max);
        Q_INVOKABLE QObject* pulse(float interval);
        Q_INVOKABLE QObject* scale(float multiplier);
        Q_INVOKABLE QObject* invert();
        Q_INVOKABLE QObject* deadZone(float min);
        Q_INVOKABLE QObject* constrainToInteger();
        Q_INVOKABLE QObject* constrainToPositiveInteger();

        // JSON route creation point
        Q_INVOKABLE QObject* filters(const QJsonValue& json);
        Q_INVOKABLE void to(const QJsonValue& json);

    private:
        void to(const Endpoint::Pointer& destination);
        void addFilter(Filter::Lambda lambda);
        void addFilter(Filter::Pointer filter);
        Mapping::Pointer _mapping;
        Route::Pointer _route;

        NewControllerScriptingInterface& _parent;
    };

}
#endif
