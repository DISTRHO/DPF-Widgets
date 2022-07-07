#!/usr/bin/env python3

ddpxs = list(range(3,51))
ddpxs.append(6.66)
ddpxs.sort()

for i in ddpxs:
  if i == 0:
    print(f'''
      @media (min-resolution: 1dppx) {{
        #canvas_wrapper {{
          scale: 1;
          left: 0;
          top: 0;
        }}
      }}''')
  elif i < 10:
    print(f'''
      @media (min-resolution: {i/10-0.01}dppx) {{
        #canvas_wrapper {{
          scale: calc(1 / {i/10});
          left: 0;
          top: calc((1 - (1 / {i/10})) / 2 * -100vh);
        }}
      }}''')
  else:
    print(f'''
      @media (min-resolution: {i/10}dppx) {{
        #canvas_wrapper {{
          scale: calc(1 / {i/10});
          left: calc((1 - (1 / {i/10})) / 2 * -100vw);
          top: calc((1 - (1 / {i/10})) / 2 * -100vh);
        }}
      }}''')
