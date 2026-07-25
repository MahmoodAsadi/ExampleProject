# Code Formatting

## Include Ordering

Apply the include-grouping style to both `.h` and `.cpp` files.

For every `.cpp` file, organize includes in this order:

1. The corresponding header for the source file.
2. Exactly one blank line.
3. Unreal Engine and third-party headers.
4. Exactly one blank line.
5. Project and plugin headers.
6. Exactly one blank line before macros or implementation code.

For every `.h` file, organize includes in this order:

1. `#pragma once`.
2. Exactly one blank line.
3. Unreal Engine and third-party headers.
4. Exactly one blank line.
5. Project and plugin headers.
6. The `*.generated.h` header, when present, as the final include in the
   project/plugin block with no blank line before it.
7. Exactly one blank line before declarations.

Keep includes alphabetically ordered within the Engine/third-party group and
within the project/plugin group. Do not mix project headers into the Engine
header group. A `*.generated.h` header is the exception to alphabetical
ordering and must remain the final include in a header file.

`.cpp` example:

```cpp
#include "Slates/SConnectedDeviceInfo.h"

#include "Styling/AppStyle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#include "IndependentInputSubsystem.h"

#define LOCTEXT_NAMESPACE "SConnectedDeviceInfo"
```

`.h` example:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"
#include "SDeviceKeyMapping.generated.h"

class SDeviceKeyMapping : public SCompoundWidget
{
};
```

## Line Endings

- Preserve Windows `CRLF` line endings in `.h` and `.cpp` files.
- Do not mix `CRLF`, `LF`, or standalone `CR` line endings within one file.
- End every file with exactly one newline character.
- The empty-looking final editor row represents the required final newline; do
  not remove it.
- Do not add a second trailing newline, which would create a real blank line at
  the end of the file.
