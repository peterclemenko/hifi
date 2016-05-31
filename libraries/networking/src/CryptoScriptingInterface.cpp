//
//  Created by Peter Clemenko III on 2016/05/31
// 
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "CryptoScriptingInterface.h"

#include "RSAKeypairGenerator.h"

void CryptoScriptingInterface::generateKeypair() {
	RSAKeypairGenerator rsaKeyPairGenerator;
	rsaKeyPairGenerator.generateKeypair();
}
