# JS Bindings CDN

You can use Soup's JavaScript bindings via `use.soup.do`.

```html
<script src="https://use.soup.do"></script>
<script>
    soup.use(function()
    {
        soup.scope(function()
        {
            let m = soup.InquiryLang.execute("base64_encode Hello");
            console.log(soup.InquiryLang.formatResultLine(m)); // SGVsbG8=
        });
    });
</script>
```
