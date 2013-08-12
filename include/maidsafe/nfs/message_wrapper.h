/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#ifndef MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
#define MAIDSAFE_NFS_MESSAGE_WRAPPER_H_

#include <string>
#include <tuple>
#include <utility>

#include "maidsafe/common/tagged_value.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace detail {

struct SourceTag;
struct DestinationTag;
typedef TaggedValue<Persona, struct detail::SourceTag> SourceTaggedValue;
typedef TaggedValue<Persona, struct detail::DestinationTag> DestinationTaggedValue;

}  // namespace detail



typedef std::tuple<MessageAction, detail::SourceTaggedValue, detail::DestinationTaggedValue,
                   MessageId, std::string> TypeErasedMessageWrapper;

template<typename Message>
struct Sender;

template<typename Message>
struct Receiver;



template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
struct MessageWrapper {
  MessageWrapper();

  // For use with new messages (a new message_id is automatically applied).
  explicit MessageWrapper(const std::string& serialised_message_in);

  // For use when handling incoming messages where the sender has already set the message_id.
  explicit MessageWrapper(const TypeErasedMessageWrapper& parsed_message_wrapper);

  MessageWrapper(const MessageWrapper& other);
  MessageWrapper(MessageWrapper&& other);
  MessageWrapper& operator=(MessageWrapper other);

  std::string Serialise() const;

  friend void swap(MessageWrapper& lhs, MessageWrapper& rhs) {
    using std::swap;
    swap(lhs.message_id, rhs.message_id);
    swap(lhs.contents, rhs.contents);
  }

  MessageId message_id;
  Contents contents;

 private:
  static const detail::SourceTaggedValue kSourceTaggedValue;
  static const detail::DestinationTaggedValue kDestinationTaggedValue;
};

TypeErasedMessageWrapper ParseMessageWrapper(const std::string& serialised_message_wrapper);



// ==================== Implementation =============================================================
namespace detail {

MessageId GetNewMessageId();

std::string SerialiseMessageWrapper(const TypeErasedMessageWrapper& message_tuple);

}  // namespace detail

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
const detail::SourceTaggedValue
    MessageWrapper<action,
                   SourcePersonaType,
                   DestinationPersonaType,
                   Contents>::kSourceTaggedValue =
        detail::SourceTaggedValue(SourcePersonaType::value);

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
const detail::DestinationTaggedValue
    MessageWrapper<action,
                   SourcePersonaType,
                   DestinationPersonaType,
                   Contents>::kDestinationTaggedValue =
        detail::DestinationTaggedValue(DestinationPersonaType::value);

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
MessageWrapper<action, SourcePersonaType, DestinationPersonaType, Contents>::MessageWrapper()
        : message_id(0),
          contents() {}

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
MessageWrapper<action, SourcePersonaType, DestinationPersonaType, Contents>::MessageWrapper(
    const std::string& serialised_message_in)
        : message_id(detail::GetNewMessageId()),
          contents(serialised_message_in) {}

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
MessageWrapper<action, SourcePersonaType, DestinationPersonaType, Contents>::MessageWrapper(
    const TypeErasedMessageWrapper& parsed_message_wrapper)
        : message_id(std::get<3>(parsed_message_wrapper)),
          contents(std::get<4>(parsed_message_wrapper)) {}

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
MessageWrapper<action, SourcePersonaType, DestinationPersonaType, Contents>::MessageWrapper(
    const MessageWrapper& other)
        : message_id(other.message_id),
          contents(other.contents) {}

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
MessageWrapper<action, SourcePersonaType, DestinationPersonaType, Contents>::MessageWrapper(
    MessageWrapper&& other)
        : message_id(std::move(other.message_id)),
          contents(std::move(other.contents)) {}

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
MessageWrapper<action, SourcePersonaType, DestinationPersonaType, Contents>&
    MessageWrapper<action, SourcePersonaType, DestinationPersonaType, Contents>::operator=(
        MessageWrapper other) {
  swap(*this, other);
  return *this;
}

template<MessageAction action,
         typename SourcePersonaType,
         typename DestinationPersonaType,
         typename Contents>
std::string MessageWrapper<action,
                           SourcePersonaType,
                           DestinationPersonaType,
                           Contents>::Serialise() const {
  return detail::SerialiseMessageWrapper(std::make_tuple(action, kSourceTaggedValue,
      kDestinationTaggedValue, message_id, contents.Serialise()));
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
