/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Type.h"
#include "TypeVisitor.h"
#include "CoreTypes.h"
#include "Symbols.h"

using namespace CPlusPlus;

Type::Type()
{ }

Type::~Type()
{ }

bool Type::isUndefinedType() const
{ return this == UndefinedType::instance(); }

bool Type::isVoidType() const
{ return asVoidType() != 0; }

bool Type::isIntegerType() const
{ return asIntegerType() != 0; }

bool Type::isFloatType() const
{ return asFloatType() != 0; }

bool Type::isPointerType() const
{ return asPointerType()  != 0; }

bool Type::isPointerToMemberType() const
{ return asPointerToMemberType() != 0; }

bool Type::isReferenceType() const
{ return asReferenceType() != 0; }

bool Type::isArrayType() const
{ return asArrayType() != 0; }

bool Type::isNamedType() const
{ return asNamedType() != 0; }

bool Type::isFunctionType() const
{ return asFunctionType() != 0; }

bool Type::isNamespaceType() const
{ return asNamespaceType() != 0; }

bool Type::isClassType() const
{ return asClassType() != 0; }

bool Type::isEnumType() const
{ return asEnumType() != 0; }

bool Type::isForwardClassDeclarationType() const
{ return asForwardClassDeclarationType() != 0; }

bool Type::isObjCClassType() const
{ return asObjCClassType() != 0; }

bool Type::isObjCProtocolType() const
{ return asObjCProtocolType() != 0; }

bool Type::isObjCMethodType() const
{ return asObjCMethodType() != 0; }

bool Type::isObjCForwardClassDeclarationType() const
{ return asObjCForwardClassDeclarationType() != 0; }

bool Type::isObjCForwardProtocolDeclarationType() const
{ return asObjCForwardProtocolDeclarationType() != 0; }

void Type::accept(TypeVisitor *visitor)
{
    if (visitor->preVisit(this))
        accept0(visitor);
    visitor->postVisit(this);
}

void Type::accept(Type *type, TypeVisitor *visitor)
{
    if (! type)
        return;

    type->accept(visitor);
}

bool Type::matchType(const Type *otherType, TypeMatcher *matcher) const
{
    return matchType0(otherType, matcher);
}

