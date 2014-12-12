TARGETS = sample1.png sample2.png sample3.png sample4.png

ALL: $(TARGETS)

$(TARGETS): %.png: %.dot
	dot -Tpng $^ -o $@

clean:
	rm -f $(TARGETS) *~

.PHONY: clean
