import Layout from "../components/Layout";
import Nav from "../components/Nav";

export default function NotFound() {
  return (
    <Layout title="Not found" home={false}>
      <Nav />
      <div className="absolute top-1/2 left-1/2 transform -translate-x-1/2 -translate-y-1/2 text-center">
        <div className="text-4xl">404 | Page not found</div>
      </div>
    </Layout>
  );
}
