/******************************************************************************
 * Copyright (C) 2017 Kitsune Ral <kitsune-ral@users.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "stateevent.h"
#include "eventcontent.h"

#include "converters.h"

namespace QMatrixClient
{
    namespace EventContent
    {
        template <typename T>
        class SimpleContent: public Base
        {
            public:
                using value_type = T;

                // The constructor is templated to enable perfect forwarding
                template <typename TT>
                SimpleContent(QString keyName, TT&& value)
                    : value(std::forward<TT>(value)), key(std::move(keyName))
                { }
                SimpleContent(const QJsonObject& json, QString keyName)
                    : Base(json)
                    , value(QMatrixClient::fromJson<T>(json[keyName]))
                    , key(std::move(keyName))
                { }

            public:
                T value;

            protected:
                QString key;

            private:
                void fillJson(QJsonObject* json) const override
                {
                    Q_ASSERT(json);
                    json->insert(key, QMatrixClient::toJson(value));
                }
        };
    } // namespace EventContent

#define DEFINE_SIMPLE_STATE_EVENT(_Name, _TypeId, _ContentType, _ContentKey) \
    class _Name : public StateEvent<EventContent::SimpleContent<_ContentType>> \
    { \
        public: \
            using content_type = _ContentType; \
            DEFINE_EVENT_TYPEID(_TypeId, _Name) \
            explicit _Name(const QJsonObject& obj) \
                : StateEvent(typeId(), obj, QStringLiteral(#_ContentKey)) \
            { } \
            template <typename T> \
            explicit _Name(T&& value) \
                : StateEvent(typeId(), matrixTypeId(), \
                             QStringLiteral(#_ContentKey), \
                             std::forward<T>(value)) \
            { } \
            auto _ContentKey() const { return content().value; } \
    }; \
    REGISTER_EVENT_TYPE(_Name) \
    // End of macro

    DEFINE_SIMPLE_STATE_EVENT(RoomNameEvent, "m.room.name", QString, name)
    DEFINE_EVENTTYPE_ALIAS(RoomName, RoomNameEvent)
    DEFINE_SIMPLE_STATE_EVENT(RoomAliasesEvent, "m.room.aliases",
                              QStringList, aliases)
    DEFINE_EVENTTYPE_ALIAS(RoomAliases, RoomAliasesEvent)
    DEFINE_SIMPLE_STATE_EVENT(RoomCanonicalAliasEvent, "m.room.canonical_alias",
                              QString, alias)
    DEFINE_EVENTTYPE_ALIAS(RoomCanonicalAlias, RoomCanonicalAliasEvent)
    DEFINE_SIMPLE_STATE_EVENT(RoomTopicEvent, "m.room.topic", QString, topic)
    DEFINE_EVENTTYPE_ALIAS(RoomTopic, RoomTopicEvent)
    DEFINE_SIMPLE_STATE_EVENT(EncryptionEvent, "m.room.encryption",
                              QString, algorithm)
    DEFINE_EVENTTYPE_ALIAS(RoomEncryption, EncryptionEvent)
} // namespace QMatrixClient
