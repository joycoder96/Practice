#include <assert.h>
#include <iostream>
#include <map>
#include <stack>
#include <unordered_map>

const char SPACE = ' ';
const char CLOSING_BRACKET = '>';
const char ATTR_DELIMITER = '=';
const char BEGIN_CLOSE_TAG = '/';
const char BEGIN_ATTR_QUERY = '~';
const char QUERY_TAGS_DELIMITER = '.';
const char DOUBLE_QUOTE = '"';

struct tag {
    std::string name;
    std::unordered_map<std::string, std::string> attrs;

    std::map<std::string, tag *> children;
};

struct query {
    std::string name;
    std::string attr;

    query *child;
};

void parse_tag(std::string_view input, tag *t) {
    // TODO: Use a better buffer data structure.
    std::string buffer;

    unsigned int i = 1;
    // Parse tag name.
    for (; input[i] != SPACE && input[i] != CLOSING_BRACKET; i++)
        buffer += input[i];
    t->name = buffer;
    buffer.clear();

    // Parse rest of attributes.
    std::pair<std::string, std::string> attr;
    bool is_parsing_name{true};
    for (; i < input.length() && input[i] != CLOSING_BRACKET; i++) {
        switch (input[i]) {
        case SPACE:
        case ATTR_DELIMITER:
        case DOUBLE_QUOTE:
            if (buffer.empty())
                continue;

            if (is_parsing_name)
                attr.first = buffer;
            else {
                attr.second = buffer;
                t->attrs.insert(attr);
            }
            is_parsing_name = !is_parsing_name;
            buffer.clear();

            break;
        default:
            buffer += input[i];
        }
    }
}

query *parse_query(std::string_view input) {
    query root;

    root.child = new query{};
    auto q = root.child;
    std::string buffer;
    for (auto ch : input) {
        switch (ch) {
        case QUERY_TAGS_DELIMITER:
        case BEGIN_ATTR_QUERY:
            q->name = buffer;
            buffer.clear();

            if (ch == QUERY_TAGS_DELIMITER) {
                q->child = new query{};
                q = q->child;
            }

            break;
        default:
            buffer += ch;
        }
    }
    q->attr = buffer;

    return root.child;
}

int main() {
    int n, q;
    std::cin >> n >> q;
    std::cin >> std::ws;

    // TODO: Use a different buffer data structure;
    std::string line;

    // Parse input into tree of tags.
    std::stack<tag *> tags;
    tags.push(new tag{});
    while (n--) {
        std::getline(std::cin, line);
        std::cin >> std::ws;
        if (line[1] == BEGIN_CLOSE_TAG) {
            auto t = tags.top();
            tags.pop();

            // There should always be a parent at top of stack (root);
            tags.top()->children.insert({t->name, t});
        } else {
            // Open a new tag and push to the stack.
            tag *t = new tag{};
            parse_tag(line, t);
            tags.push(t);
        }
    }
    assert(tags.size() == 1);

    // Parse and answer queries.
    tag *t;
    while (q--) {
        std::getline(std::cin, line);
        std::cin >> std::ws;
        auto q = parse_query(line);
        for (t = tags.top(); t && q->child;
             t = t->children[q->name], q = q->child)
            ;
        if (t)
            t = t->children[q->name];

        if (t && t->attrs.find(q->attr) != t->attrs.end())
            std::cout << t->attrs[q->attr] << '\n';
        else
            std::cout << "Not Found!" << '\n';
    }

    return 0;
}
