//
//  CryptoScriptingInterface.h
//  libraries/networking/src
//
//  Created by Peter Clemenko III on 2016/05/31
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//


#ifndef hifi_networking_CryptoScriptingInterface_h
#define hifi_networking_CryptoScriptingInterface_h

#include <QtCore/QObject>

#include <DependencyManager.h>

class CryptoScriptingInterface : public QObject, public Dependency {
	Q_OBJECT
public:
	Q_INVOKABLE void generateKeypair();

};


#endif
