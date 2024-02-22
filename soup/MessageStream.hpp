#pragma once

#include <unordered_set>

namespace soup
{
	// Intended for networked environments where clients want to pull from a message/event source without keeping track of state themselves.
	template <typename MessageT, typename RecipientT = size_t>
	struct MessageStream
	{
		struct Entry
		{
			std::unordered_set<RecipientT> recipients{}; // only those for whom this is the most recent message
			MessageT msg;

			Entry(MessageT&& msg)
				: msg(std::move(msg))
			{
			}
		};
		std::vector<Entry> entries{}; // most recent messages first

		auto add(MessageT&& msg)
		{
			return entries.emplace(entries.begin(), std::move(msg));
		}

		[[nodiscard]] const MessageT* getOldestUnread(RecipientT recipient)
		{
			SOUP_IF_UNLIKELY (entries.empty())
			{
				// No messages in this stream.
				return nullptr;
			}
			auto e = entries.begin();
			// Work forwards (backwards in time) to find what the last message is this recipient got.
			for (; e != entries.end(); ++e)
			{
				if (auto i = e->recipients.find(recipient); i != e->recipients.end())
				{
					// This is the most recent message for this recipient.
					SOUP_IF_LIKELY (e == entries.begin())
					{
						// It is also the most recent message in general.
						return nullptr;
					}
					e->recipients.erase(i); // Remove recipient from this message.
					break;
				}
			}
			--e;
			e->recipients.emplace(recipient);
			return &e->msg;
		}

		// Returned vector is in chronological order.
		[[nodiscard]] std::vector<const MessageT*> getAllUnread(RecipientT recipient)
		{
			SOUP_IF_UNLIKELY (entries.empty())
			{
				// No messages in this stream.
				return {};
			}
			auto e = entries.begin();
			// Work forwards (backwards in time) to find what the last message is this recipient got.
			for (; e != entries.end(); ++e)
			{
				if (auto i = e->recipients.find(recipient); i != e->recipients.end())
				{
					// This is the most recent message for this recipient.
					SOUP_IF_LIKELY (e == entries.begin())
					{
						// It is also the most recent message in general.
						return {};
					}
					e->recipients.erase(i); // Remove recipient from this message.
					break;
				}
			}
			std::vector<const MessageT*> res{};
			// Work backwards (forwards in time) to get all unread messages.
			while (e-- != entries.begin())
			{
				res.emplace_back(&e->msg);
			}
			entries.begin()->recipients.emplace(recipient);
			return res;
		}
	};
}
