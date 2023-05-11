# gglm Support for RedPajama Model

## Ackonwledgement 

We highly appreciate the great effort from the fork of [gptneox.cpp](https://github.com/byroneverson/gptneox.cpp). Our support of the RedPajama Model is mainly based on this implementation. We extend the model configure and fixed a bug when setting use_parallel_residual flag to False in their original implementation. We also extend the chat model for RedPajama.

## Usage:

### RedPajama Chat model:

- Make the code:

        make redpajama-chat quantize-gptneox


- Prepare the RedPajama model (f16 and q4_0) for gglm:

        bash ./examples/redpajama/scripts/install-RedPajama-INCITE-Chat-3B-v1.sh

- Run RedPajama chat model (fp16):

        ./redpajama-chat -m ./examples/redpajama/models/pythia/ggml-RedPajama-INCITE-Chat-3B-v1-f16.bin \
        -c 2048 \
        -b 128 \
        -n 1 \
        -t 8 \
        --instruct \
        --color \
        --top_k 30 \
        --top_p 0.95 \
        --temp 0.8 \
        --repeat_last_n 3 \
        --repeat_penalty 1.1 \
        --seed 0

    Note that you may need to install torch and transformers to run the above scripts, e.g.:
        
        pip install torch==2.0.0
        pip install transformers==4.28.1


- Run RedPajama chat model (q4_0):

        ./redpajama-chat -m ./examples/redpajama/models/pythia/ggml-RedPajama-INCITE-Chat-3B-v1-q4_0.bin \
        -c 2048 \
        -b 128 \
        -n 1 \
        -t 8 \
        --instruct \
        --color \
        --top_k 30 \
        --top_p 0.95 \
        --temp 0.8 \
        --repeat_last_n 3 \
        --repeat_penalty 1.1 \
        --seed 0

- Run other quantized version of RedPajama Chat model (Make sure you get the f16 model prepared before you run this):

  - Make the code to quantize the model if you have not:

        make quantize-gptneox

  - Generate the quantized model, the supported types include: q4_0, q4_1, q4_2, q5_0, q5_1, and q8_0. For example, to run q4_1, you need to do the following convertion:

        python ./examples/redpajama/scripts/quantize-gptneox.py ./examples/redpajama/models/pythia/ggml-RedPajama-INCITE-Chat-3B-v1-f16.bin --quantize-output-type q4_1

  - Then you can chat with the quantized model:

        ./redpajama-chat -m ./examples/redpajama/models/pythia/ggml-RedPajama-INCITE-Chat-3B-v1-q4_1.bin \
        -c 2048 \
        -b 128 \
        -n 1 \
        -t 8 \
        --instruct \
        --color \
        --top_k 30 \
        --top_p 0.95 \
        --temp 0.8 \
        --repeat_last_n 3 \
        --repeat_penalty 1.1 \
        --seed 0




### RedPajama Base/Instruct model:

- Make the code:

        make redpajama quantize-gptneox


- Prepare the RedPajama Base/Instruct model (f16 and q4_0) for gglm:

        bash ./examples/redpajama/scripts/install-RedPajama-INCITE-Base-3B-v1.sh

        # Or 

        bash ./examples/redpajama/scripts/install-RedPajama-INCITE-Instruct-3B-v1.sh

- Run other quantize version of RedPajama Base/Instruct model (Make sure you get the f16 model prepared before you run this). Then you can generate the quantized model, the supported types include: q4_0, q4_1, q4_2, q5_0, q5_1, and q8_0. For example, to run q4_1, you need to do the following convertion, e.g for RedPajama-Base q8_0:

        python ./examples/redpajama/scripts/quantize-gptneox.py ./examples/redpajama/models/pythia/ggml-RedPajama-INCITE-Base-3B-v1-f16.bin --quantize-output-type q8_0

- Run RedPajama Base/Instruct model (e.g., RedPajama-Instruct q8_0) :

        ./redpajama -m ./examples/redpajama/models/pythia/ggml-RedPajama-INCITE-Instruct-3B-v1-q8_0.bin \
        -c 2048 \
        -b 128 \
        -n 1 \
        -t 8 \
        --color \
        --top_k 30 \
        --top_p 0.95 \
        --temp 0.8 \
        --repeat_last_n 3 \
        --repeat_penalty 1.1 \
        --seed 0 \
        --n_predict 256 \
        --verbose-prompt \
        -p "How to schedule a tour to Anfield:"


## Attribution

The following files are covered by a MIT license and were taken from:

https://github.com/byroneverson/gptneox.cpp

Thank you Byron.

```
common-gptneox.cpp	
copy-gptneox.cpp	
gptneox.cpp		
quantize-gptneox.cpp
common-gptneox.h	
gptneox-util.h		
gptneox.h
convert_gptneox_to_ggml.py
quantize-gptneox.py
```