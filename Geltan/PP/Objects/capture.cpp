/* libgeltan - Qt based payment service library
 * Copyright (C) 2016 Buschtrommel / Matthias Fehring
 * Contact: https://www.buschmann23.de
 *
 * Geltan/PP/Objects/capture.cpp
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "capture_p.h"
#include <Geltan/PP/Objects/paymentamount.h>
#include <Geltan/PP/Objects/currency.h>
#include <Geltan/PP/Objects/link.h>
#include <QJsonDocument>
#include <QJsonArray>
#ifdef QT_DEBUG
#include <QtDebug>
#endif

using namespace Geltan;
using namespace PP;


Capture::Capture(QObject *parent) :
    QObject(parent), d_ptr(new CapturePrivate(this))
{
}


Capture::Capture(const QJsonDocument &json, QObject *parent) :
    QObject(parent), d_ptr(new CapturePrivate(this))
{
    loadFromJson(json);
}


Capture::Capture(const QJsonObject &json, QObject *parent) :
    QObject(parent), d_ptr(new CapturePrivate(this))
{
    loadFromJson(json);
}



Capture::~Capture()
{
}


QString Capture::id() const { Q_D(const Capture); return d->id; }


PaymentAmount *Capture::amount() const { Q_D(const Capture); return d->amount; }

void Capture::setAmount(PaymentAmount *nAmount)
{
    Q_D(Capture); 
    if (nAmount != d->amount) {
        d->amount = nAmount;
#ifdef QT_DEBUG
        qDebug() << "Changed amount to" << d->amount;
#endif
        Q_EMIT amountChanged(amount());
    }
}




bool Capture::isFinalCapture() const { Q_D(const Capture); return d->isFinalCapture; }

void Capture::setIsFinalCapture(bool nIsFinalCapture)
{
    Q_D(Capture); 
    if (nIsFinalCapture != d->isFinalCapture) {
        d->isFinalCapture = nIsFinalCapture;
#ifdef QT_DEBUG
        qDebug() << "Changed isFinalCapture to" << d->isFinalCapture;
#endif
        Q_EMIT isFinalCaptureChanged(isFinalCapture());
    }
}




PayPal::StateType Capture::state() const { Q_D(const Capture); return d->state; }



PayPal::ReasonCode Capture::reasonCode() const { Q_D(const Capture); return d->reasonCode; }



QString Capture::parentPayment() const { Q_D(const Capture); return d->parentPayment; }



QString Capture::invoiceNumber() const { Q_D(const Capture); return d->invoiceNumber; }

void Capture::setInvoiceNumber(const QString &nInvoiceNumber)
{
    Q_D(Capture); 
    if (nInvoiceNumber != d->invoiceNumber) {
        d->invoiceNumber = nInvoiceNumber;
#ifdef QT_DEBUG
        qDebug() << "Changed invoiceNumber to" << d->invoiceNumber;
#endif
        Q_EMIT invoiceNumberChanged(invoiceNumber());
    }
}




Currency *Capture::transactionFee() const { Q_D(const Capture); return d->transactionFee; }

void Capture::setTransactionFee(Currency *nTransactionFee)
{
    Q_D(Capture); 
    if (nTransactionFee != d->transactionFee) {
        d->transactionFee = nTransactionFee;
#ifdef QT_DEBUG
        qDebug() << "Changed transactionFee to" << d->transactionFee;
#endif
        Q_EMIT transactionFeeChanged(transactionFee());
    }
}




QDateTime Capture::createTime() const { Q_D(const Capture); return d->createTime; }




QDateTime Capture::updateTime() const { Q_D(const Capture); return d->updateTime; }


QList<Link*> Capture::links() const { Q_D(const Capture); return d->links; }


QUrl Capture::getLinkURL(const QString &rel) const
{
    const Link *l = getLink(rel);
    if (l) {
        return l->href();
    } else {
        return QUrl();
    }
}



Link* Capture::getLink(const QString &rel) const
{
    Q_D(const Capture);

    if (d->links.isEmpty()) {
        return nullptr;
    }

    for (Link *l : d->links) {
        if (l->rel() == rel) {
            return l;
        }
    }

    return nullptr;
}




QVariantMap Capture::toVariant()
{
    Q_D(Capture);

    QVariantMap map;

    if (amount()) {
        d->addMapToVariantMap(&map, QStringLiteral("amount"), amount()->toVariant());
    }
    map.insert(QStringLiteral("is_final_capture"), isFinalCapture());
    d->addStringToVariantMap(&map, QStringLiteral("invoice_number"), invoiceNumber(), 127);
    if (transactionFee()) {
        d->addMapToVariantMap(&map, QStringLiteral("transaction_feee"), transactionFee()->toVariant());
    }

    return map;
}


QJsonObject Capture::toJsonObject()
{
    return QJsonObject::fromVariantMap(this->toVariant());
}



void Capture::loadFromJson(const QJsonDocument &json)
{
    loadFromJson(json.object());
}



void Capture::loadFromJson(const QJsonObject &json)
{
    if (json.isEmpty()) {
        return;
    }

    Q_D(Capture);

    QScopedPointer<const PPEnumsMap> em(new PPEnumsMap);

    d->setId(json.value(QStringLiteral("id")).toString());

    const QJsonObject ao = json.value(QStringLiteral("amount")).toObject();
    PaymentAmount *oldAo = amount();
    if (!ao.isEmpty()) {
        if (oldAo) {
            oldAo->loadFromJson(ao);
        } else {
            setAmount(new PaymentAmount(ao, this));
        }
    } else {
        setAmount(nullptr);
        delete oldAo;
    }

    setIsFinalCapture(json.value(QStringLiteral("is_final_capture")).toString() == QLatin1String("true"));

    d->setState(em->stateTypeTokenToEnum(json.value(QStringLiteral("state")).toString()));

    d->setReasonCode(em->reasonCodeTokenToEnum(json.value(QStringLiteral("reason_code")).toString()));

    d->setParentPayment(json.value(QStringLiteral("parent_payment")).toString());

    setInvoiceNumber(json.value(QStringLiteral("invoice_number")).toString());

    const QJsonObject tf = json.value(QStringLiteral("transaction_fee")).toObject();
    Currency *oldTf = transactionFee();
    if (!tf.isEmpty()) {
        if (oldTf) {
            oldTf->loadFromJson(tf);
        } else {
            setTransactionFee(new Currency(tf, this));
        }
    } else {
        setTransactionFee(nullptr);
        delete oldTf;
    }

    const QString ct = json.value(QStringLiteral("create_time")).toString();
    if (!ct.isEmpty()) {
        d->setCreateTime(QDateTime::fromString(ct, Qt::ISODate));
    } else {
        d->setCreateTime(QDateTime());
    }

    const QString ut = json.value(QStringLiteral("update_time")).toString();
    if (!ut.isEmpty()) {
        d->setUpdateTime(QDateTime::fromString(ut, Qt::ISODate));
    } else {
        d->setUpdateTime(QDateTime());
    }

    const QJsonArray la = json.value(QStringLiteral("links")).toArray();
    d->clearLinks();
    Q_EMIT linksChanged(links());
    if (!la.isEmpty()) {
        QList<Link*> linksToAdd;
        QJsonArray::const_iterator i = la.constBegin();
        while (i != la.constEnd()) {
            linksToAdd.append(new Link(i->toObject(), this));
            ++i;
        }
        d->setLinks(linksToAdd);
    }
}
