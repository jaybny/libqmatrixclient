/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
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

#include "roommemberevent.h"

#include "converters.h"
#include "logging.h"

#include <array>

using namespace QMatrixClient;

static const std::array<QString, 5> membershipStrings = { {
    QStringLiteral("invite"), QStringLiteral("join"),
    QStringLiteral("knock"), QStringLiteral("leave"),
    QStringLiteral("ban")
} };

namespace QMatrixClient
{
    template <>
    struct FromJson<MembershipType>
    {
        MembershipType operator()(const QJsonValue& jv) const
        {
            const auto& membershipString = jv.toString();
            for (auto it = membershipStrings.begin();
                    it != membershipStrings.end(); ++it)
                if (membershipString == *it)
                    return MembershipType(it - membershipStrings.begin());

            qCWarning(EVENTS) << "Unknown MembershipType: " << membershipString;
            return MembershipType::Undefined;
        }
    };
}

MemberEventContent::MemberEventContent(const QJsonObject& json)
    : membership(fromJson<MembershipType>(json["membership"_ls]))
    , isDirect(json["is_direct"_ls].toBool())
    , displayName(json["displayname"_ls].toString())
    , avatarUrl(json["avatar_url"_ls].toString())
{ }

void MemberEventContent::fillJson(QJsonObject* o) const
{
    Q_ASSERT(o);
    Q_ASSERT_X(membership != MembershipType::Undefined, __FUNCTION__,
             "The key 'membership' must be explicit in MemberEventContent");
    if (membership != MembershipType::Undefined)
        o->insert(QStringLiteral("membership"), membershipStrings[membership]);
    o->insert(QStringLiteral("displayname"), displayName);
    if (avatarUrl.isValid())
        o->insert(QStringLiteral("avatar_url"), avatarUrl.toString());
}
